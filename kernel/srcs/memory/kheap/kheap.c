/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 00:17:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 23:04:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/ealloc.h> // Early Alloc (ealloc, ealloc_aligned)
#include <mm/kheap.h>  // Kernel Heap
#include <mm/mm.h>	   // Memory Management
#include <mm/mmu.h>	   // Memory Management Unit (MMU / Paging)
#include <mm/mmuf.h>   // Memory Management Unit Frames (MMU Frames)

#include <assert.h> // assert
#include <stddef.h> // NULL

#include <system/serial.h> // NULL

/* Structure globale représentant le heap */
static heap_t *kernel_heap = NULL;

/* Static heap structures */
static heap_t kernel_heap_struct;

/**
 * @brief Initializes a heap block with given parameters.
 *
 * @param block Pointer to the heap block to initialize.
 * @param size Size of the block.
 * @param is_free Free flag of the block.
 */
static void initialize_block(heap_block_t *block, size_t size, bool is_free) {
	block->size = size;
	block->is_free = is_free;
	block->next = NULL;
	block->prev = NULL;
	block->magic = HEAP_BLOCK_MAGIC;
}

/**
 * @brief Splits a block into two if it's larger than needed.
 *
 * @param block The block to split.
 * @param size The size to split the block into.
 */
static void split_block(heap_block_t *block, size_t size) {
	if (block->size <= size + sizeof(heap_block_t) + ALIGNMENT) {
		qemu_printf("split_block: Not enough space to split block at %p\n", (void *)block);
		return; // Not enough space to split while maintaining alignment
	}

	uintptr_t block_addr = (uintptr_t)block;
	heap_block_t *new_block = (heap_block_t *)(block_addr + sizeof(heap_block_t) + size);
	initialize_block(new_block, block->size - size - sizeof(heap_block_t), true);
	new_block->next = block->next;
	new_block->prev = block;

	if (new_block->next) {
		new_block->next->prev = new_block;
	} else {
		kernel_heap->last = new_block;
	}

	block->size = size;
	block->next = new_block;

	qemu_printf("split_block: Split block at %p into allocated block and free block at %p\n", (void *)block, (void *)new_block);
}

/**
 * @brief Coalesces adjacent free blocks to prevent fragmentation.
 *
 * @param block The block to coalesce.
 */
static void coalesce(heap_block_t *block) {
	// Coalesce with next block if possible
	if (block->next && block->next->is_free) {
		if (block->next->magic != HEAP_BLOCK_MAGIC) {
			qemu_printf("coalesce: Next block at %p has invalid magic number!\n", (void *)block->next);
			__PANIC("Heap corruption detected during coalesce (next block magic mismatch)");
		}

		qemu_printf("coalesce: Coalescing block at %p with next block at %p\n", (void *)block, (void *)block->next);
		block->size += sizeof(heap_block_t) + block->next->size;
		block->next = block->next->next;
		if (block->next) {
			block->next->prev = block;
		} else {
			kernel_heap->last = block;
		}
	}

	// Coalesce with previous block if possible
	if (block->prev && block->prev->is_free) {
		if (block->prev->magic != HEAP_BLOCK_MAGIC) {
			qemu_printf("coalesce: Previous block at %p has invalid magic number!\n", (void *)block->prev);
			__PANIC("Heap corruption detected during coalesce (previous block magic mismatch)");
		}

		qemu_printf("coalesce: Coalescing block at %p with previous block at %p\n", (void *)block, (void *)block->prev);
		block->prev->size += sizeof(heap_block_t) + block->size;
		block->prev->next = block->next;
		if (block->next) {
			block->next->prev = block->prev;
		} else {
			kernel_heap->last = block->prev;
		}
	}
}

/**
 * @brief Finds a free block using the first-fit strategy.
 *
 * @param heap The heap to search.
 * @param size The size of the block to find.
 * @return A pointer to the free block, or NULL if none found.
 */
static heap_block_t *find_free_block(heap_t *heap, size_t size) {
	heap_block_t *current = heap->first;

	while (current) {
		if (current->is_free && current->size >= size) {
			if (current->magic != HEAP_BLOCK_MAGIC) {
				__PANIC("Heap corruption detected during find_free_block (magic mismatch)");
			}
			return current; // Return the first suitable block found
		}
		current = current->next;
	}

	return NULL; // No suitable block found
}

/**
 * @brief Requests space by expanding the heap.
 *
 * @param heap The heap to expand.
 * @param size The size of the space to request.
 * @return A pointer to the new block, or NULL on failure.
 */
static heap_block_t *request_space(heap_t *heap, size_t size) {
	// Calculate the number of pages needed
	size_t total_size = sizeof(heap_block_t) + size;
	size_t pages_needed = (total_size + PAGE_SIZE - 1) / PAGE_SIZE;

	qemu_printf("request_space: Requesting %ld pages for size %ld bytes\n", pages_needed, size);

	// Check if heap exceeds maximum size
	if (heap->size + (pages_needed * PAGE_SIZE) > HEAP_MAX_SIZE) {
		qemu_printf("request_space: Heap expansion exceeds maximum size\n");
		return NULL; // Cannot expand heap beyond maximum size
	}

	// **Directly create and allocate pages without using mmu_map_page**
	for (size_t i = 0; i < pages_needed; i++) {
		uintptr_t addr = HEAP_START + heap->size + (i * PAGE_SIZE);
		page_t *page = mmu_create_page(addr, heap->dir, 1); // is_kernel=1
		if (!page) {
			qemu_printf("request_space: Failed to create page at %p\n", (void *)addr);
			// Rollback previously allocated pages
			for (size_t j = 0; j < i; j++) {
				uintptr_t rollback_addr = HEAP_START + heap->size + (j * PAGE_SIZE);
				mmu_destroy_page(rollback_addr, heap->dir);
			}
			return NULL; // Failed to create page
		}
		allocate_frame(page, 1, 1); // is_kernel=1, is_writeable=1
		page->present = 1;
		page->rw = 1;
		page->user = 0;
		mmu_flush_tlb_entry(addr);
	}

	heap->size += pages_needed * PAGE_SIZE;

	// Initialize the new block at the end of the heap
	heap_block_t *block = (heap_block_t *)(HEAP_START + heap->size - (pages_needed * PAGE_SIZE));
	initialize_block(block, (pages_needed * PAGE_SIZE) - sizeof(heap_block_t), true);
	block->next = NULL;
	block->prev = heap->last;

	if (heap->last) {
		heap->last->next = block;
	}

	heap->last = block;

	qemu_printf("request_space: Mapped %ld pages, new heap size: %ld bytes\n", pages_needed, heap->size);

	return block;
}

/**
 * @brief Creates a new heap.
 *
 * @param dir The page directory to associate with the heap.
 * @param size The initial size of the heap.
 * @return A pointer to the created heap, or NULL on failure.
 */
static heap_t *create_heap(page_directory_t *dir, size_t size) {
	// Initialiser la structure statique du heap
	heap_t *heap = &kernel_heap_struct;
	memset(heap, 0, sizeof(heap_t));

	heap->size = 0;
	heap->dir = dir;

	// Calculer le nombre de tables de pages nécessaires
	size_t pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;

	for (size_t i = 0; i < pages_needed; i++) {
		uintptr_t addr = HEAP_START + i * PAGE_SIZE;
		// Allouer une nouvelle table de pages
		uint32_t new_table_phys;
		page_table_t *new_table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &new_table_phys);
		if (new_table == NULL || new_table_phys == 0) {
			printk("create_heap: Échec de l'allocation de la table de pages à 0x%p\n", (void *)addr);
			qemu_printf("create_heap: Échec de l'allocation de la table de pages à 0x%p\n", (void *)addr);
			// Rollback des allocations précédentes
			for (size_t j = 0; j < i; j++) {
				uintptr_t rollback_addr = HEAP_START + j * PAGE_SIZE;
				mmu_destroy_page(rollback_addr, dir);
			}
			__PANIC("Failed to create heap page table");
		}

		// Initialiser la table de pages
		memset(new_table, 0, sizeof(page_table_t));

		// Allouer et initialiser un cadre pour la table de pages
		allocate_frame(&new_table->pages[0], 1, 1); // is_kernel=1, is_writeable=1
		if (new_table->pages[0].frame == 0) {
			printk("create_heap: Échec de l'allocation du cadre pour la table de pages à 0x%p\n", (void *)addr);
			qemu_printf("create_heap: Échec de l'allocation du cadre pour la table de pages à 0x%p\n", (void *)addr);
			kfree(new_table);
			// Rollback des allocations précédentes
			for (size_t j = 0; j < i; j++) {
				uintptr_t rollback_addr = HEAP_START + j * PAGE_SIZE;
				mmu_destroy_page(rollback_addr, dir);
			}
			__PANIC("Failed to allocate frame for heap page table");
		}

		// Copier les flags depuis la première page
		new_table->pages[0].present = 1;
		new_table->pages[0].rw = 1;
		new_table->pages[0].user = 0;
		new_table->pages[0].frame = new_table->pages[0].frame; // Déjà défini par allocate_frame

		// Assigner la table de pages au répertoire
		dir->tables[i] = new_table;
		dir->tablesPhysical[i] = (new_table->pages[0].frame * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITE; // PAGE_USER n'est pas ajouté pour les pages du noyau

		// Actualiser le TLB
		mmu_flush_tlb();
	}

	heap->size = pages_needed * PAGE_SIZE;

	// Initialiser le premier bloc de heap
	heap_block_t *initial_block = (heap_block_t *)HEAP_START;
	initialize_block(initial_block, size - sizeof(heap_block_t) * 2, true);
	heap->first = initial_block;
	heap->last = initial_block;

	qemu_printf("create_heap: Heap créé à 0x%p avec une taille de %ld bytes\n", (void *)HEAP_START, size);
	printk("create_heap: Heap créé à 0x%p avec une taille de %ld bytes\n", (void *)HEAP_START, size);

	return heap;
}

/**
 * @brief Initializes the heap.
 *
 * @param dir The page directory to associate with the heap.
 */
void initialize_heap(page_directory_t *dir) {
	kernel_heap = create_heap(dir, HEAP_INITIAL_SIZE);
	if (kernel_heap == NULL) {
		// Handle heap creation failure
		__PANIC("Heap initialization failed");
	}
	printk("initialize_heap: Heap initialized successfully\n");
	qemu_printf("initialize_heap: Heap initialized successfully\n");
}

/**
 * @brief Allocates memory from the heap.
 *
 * @param size The size of the memory to allocate.
 * @param align Whether to align the memory (1 for alignment, 0 otherwise).
 * @return A pointer to the allocated memory, or NULL on failure.
 */
void *kheap_alloc(size_t size, uint8_t align) {
	if (size == 0) {
		return NULL;
	}

	// Adjust size for alignment if necessary
	size_t aligned_size = size;
	if (align) {
		aligned_size = ALIGN_UP(size, ALIGNMENT); // Use defined ALIGNMENT
		qemu_printf("kheap_alloc: Adjusted size to %ld bytes for alignment %d\n", aligned_size, ALIGNMENT);
	}

	// Find a suitable free block
	heap_block_t *block = find_free_block(kernel_heap, aligned_size);
	if (!block) {
		block = request_space(kernel_heap, aligned_size);
		if (!block) {
			qemu_printf("kheap_alloc: Heap expansion failed for size %ld bytes\n", aligned_size);
			return NULL; // Heap expansion failed
		}
	}

	// Calculate the aligned address within the block
	uintptr_t addr = (uintptr_t)block + sizeof(heap_block_t);
	uintptr_t aligned_addr = align ? ALIGN_UP(addr, ALIGNMENT) : addr;
	size_t padding = aligned_addr - addr;

	// If padding is needed, split the block
	if (padding > 0) {
		if (padding >= sizeof(heap_block_t) + ALIGNMENT) {
			// Split the block into padding and the aligned block
			split_block(block, padding - sizeof(heap_block_t));
			block = block->next; // The aligned block
		} else {
			// Not enough space to align the block, find another block or request more space
			block = find_free_block(kernel_heap, aligned_size + padding);
			if (!block) {
				block = request_space(kernel_heap, aligned_size + padding);
				if (!block) {
					qemu_printf("kheap_alloc: Heap expansion failed for size %ld bytes\n", aligned_size + padding);
					return NULL; // Heap expansion failed
				}
			}
			// Recalculate the aligned address within the new block
			addr = (uintptr_t)block + sizeof(heap_block_t);
			aligned_addr = align ? ALIGN_UP(addr, ALIGNMENT) : addr;
			padding = aligned_addr - addr;
			if (padding > 0) {
				split_block(block, padding - sizeof(heap_block_t));
				block = block->next; // The aligned block
			}
		}
	}

	// Allocate the aligned block
	block->is_free = false;
	if (block->size >= aligned_size + sizeof(heap_block_t) + ALIGNMENT) {
		split_block(block, aligned_size);
	}

	qemu_printf("kheap_alloc: Allocated block at %p with size %ld bytes\n", (void *)aligned_addr, aligned_size);

	return (void *)aligned_addr;
}

/**
 * @brief Frees allocated memory back to the heap.
 *
 * @param ptr The pointer to the memory to free.
 */
void kheap_free(void *ptr) {
	if (!ptr) {
		return;
	}

	// Retrieve the block metadata
	heap_block_t *block = (heap_block_t *)((uintptr_t)ptr - sizeof(heap_block_t));

	// Validate the block
	if ((uintptr_t)block < HEAP_START || (uintptr_t)block >= (HEAP_START + kernel_heap->size)) {
		qemu_printf("kheap_free: Pointer %p out of heap bounds.\n", ptr);
		return; // Pointer is out of heap bounds
	}

	if (block->magic != HEAP_BLOCK_MAGIC) {
		qemu_printf("kheap_free: Block at %p has invalid magic number!\n", (void *)block);
		__PANIC("Heap corruption detected during free (magic mismatch)");
	}

	// Mark the block as free
	block->is_free = true;

	qemu_printf("kheap_free: Freed memory at %p\n", ptr);

	// Coalesce adjacent free blocks to prevent fragmentation
	coalesce(block);
}

/**
 * @brief Intermediate allocation function handling alignment and physical addresses.
 *
 * @param size The size of the memory to allocate.
 * @param align Whether to align the memory (1 for alignment, 0 otherwise).
 * @param phys Pointer to store the physical address (optional).
 * @return A pointer to the allocated memory, or NULL on failure.
 */
static void *intermediate_alloc(size_t size, uint8_t align, uint32_t *phys) {
	/* If the kernel heap is initialized, we use it */
	if (kernel_heap) {
		void *addr = kheap_alloc(size, align);
		if (phys) {
			page_t *page = mmu_get_page((uintptr_t)addr, mmu_get_kernel_directory());
			if (page)
				*phys = page->frame * PAGE_SIZE + ((uintptr_t)addr & 0xFFF);
			else
				*phys = 0;
		}
		return addr;
	}
	/* If the kernel heap is not initialized, we use the early alloc */
	else {
		uintptr_t placement = get_placement_addr();
		if (align && (placement & (ALIGNMENT - 1))) {
			placement = ALIGN_UP(placement, ALIGNMENT);
			set_placement_addr(placement);
		}
		if (phys) {
			*phys = placement;
		}
		/* Don't need to store data in block -> reserved for system */
		void *addr = (void *)ealloc(size);
		set_placement_addr(placement + size);
		return addr;
	}
}

/* Memory Allocation Functions */

void *kmalloc(size_t size) {
	return intermediate_alloc(size, 0, NULL);
}

void *kmalloc_a(size_t size) {
	return intermediate_alloc(size, 1, NULL);
}

void *kmalloc_p(size_t size, uint32_t *phys) {
	return intermediate_alloc(size, 0, phys);
}

void *kmalloc_ap(size_t size, uint32_t *phys) {
	return intermediate_alloc(size, 1, phys);
}

void kfree(void *p) {
	if (kernel_heap) {
		kheap_free(p);
	}
}

void *kcalloc(size_t num, size_t size) {
	void *p = kmalloc(num * size);
	if (!p) return NULL;
	memset(p, 0, num * size);
	return p;
}

void *krealloc(void *p, size_t size) {
	if (!p) {
		return kmalloc(size);
	}

	if (size == 0) {
		kfree(p);
		return NULL;
	}

	// Retrieve the block metadata
	heap_block_t *block = (heap_block_t *)((uintptr_t)p - sizeof(heap_block_t));

	// Validate the block
	if ((uintptr_t)block < HEAP_START || (uintptr_t)block >= (HEAP_START + kernel_heap->size)) {
		qemu_printf("krealloc: Pointer %p out of heap bounds.\n", p);
		return NULL; // Invalid pointer
	}

	if (block->magic != HEAP_BLOCK_MAGIC) {
		qemu_printf("krealloc: Block at %p has invalid magic number!\n", (void *)block);
		__PANIC("Heap corruption detected during realloc (magic mismatch)");
	}

	size_t old_size = block->size;
	if (old_size >= size) {
		// Current block is sufficient
		if (block->size >= size + sizeof(heap_block_t) + ALIGNMENT) {
			split_block(block, size);
		}
		return p;
	} else {
		// Allocate new block
		void *new_ptr = kmalloc(size);
		if (!new_ptr) {
			return NULL; // Allocation failed
		}
		memcpy(new_ptr, p, old_size);
		kfree(p);
		return new_ptr;
	}
}

size_t ksize(void *p) {
	if (!p) return 0;
	heap_block_t *block = (heap_block_t *)((uintptr_t)p - sizeof(heap_block_t));
	if (block->magic != HEAP_BLOCK_MAGIC) {
		qemu_printf("ksize: Block at %p has invalid magic number!\n", (void *)block);
		__PANIC("Heap corruption detected during ksize (magic mismatch)");
	}
	return block->size;
}