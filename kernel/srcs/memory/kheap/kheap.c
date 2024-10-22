/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 00:17:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 16:56:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/ealloc.h> // Early Alloc (ealloc, ealloc_aligned)
#include <mm/kheap.h>  // Kernel Heap
#include <mm/mm.h>	   // Memory Management
#include <mm/mmu.h>	   // Memory Management Unit (MMU / Paging)
#include <mm/mmuf.h>   // Memory Management Unit Frames (MMU Frames)
#include <mm/pagetable_pool.h>

#include <assert.h> // assert
#include <stddef.h> // NULL

#include <system/serial.h> // NULL

// Définir le maximum order basé sur HEAP_MAX_SIZE
#define MAX_ORDER 12 // 2^12 * 4 KB = 16 MB

static heap_t *kernel_heap = NULL;
static heap_t kernel_heap_struct;
// Initialiser le buddy allocator
static void buddy_init(heap_t *heap) {
	for (int i = 0; i <= MAX_ORDER; i++) {
		heap->free_lists[i] = NULL;
	}

	// Créer le bloc libre initial
	size_t initial_size = 1 << MAX_ORDER; // 16 MB
	buddy_block_t *initial = (buddy_block_t *)kmalloc(sizeof(buddy_block_t));
	if (!initial) {
		qemu_printf("buddy_init: Échec de l'allocation du bloc initial.\n");
		__PANIC("Échec de l'initialisation du buddy allocator");
	}
	initial->size = initial_size;
	initial->is_free = true;
	initial->next = heap->free_lists[MAX_ORDER];
	heap->free_lists[MAX_ORDER] = initial;

	qemu_printf("buddy_init: Buddy allocator initialisé avec un bloc de %ld bytes\n", initial_size);
}

// Diviser un bloc en deux buddies
static void split_block(int order) {
	if (order <= 0 || order > MAX_ORDER) return;

	buddy_block_t *block = kernel_heap->free_lists[order];
	if (!block) return;

	// Retirer le bloc de la liste libre
	kernel_heap->free_lists[order] = block->next;

	// Créer deux buddies
	size_t split_size = 1 << (order - 1); // Taille du buddy après division
	buddy_block_t *buddy1 = block;
	buddy_block_t *buddy2 = (buddy_block_t *)((uintptr_t)block + split_size);

	// Initialiser les buddies
	buddy1->size = split_size;
	buddy1->is_free = true;
	buddy1->next = kernel_heap->free_lists[order - 1];
	kernel_heap->free_lists[order - 1] = buddy1;

	buddy2->size = split_size;
	buddy2->is_free = true;
	buddy2->next = kernel_heap->free_lists[order - 1];
	kernel_heap->free_lists[order - 1] = buddy2;

	qemu_printf("split_block: Bloc divisé en deux buddies de %ld bytes chacun\n", split_size);
}

// Trouver l'ordre approprié pour l'allocation
static int find_order(size_t size) {
	int order = 0;
	size_t total_size = size + sizeof(buddy_block_t);
	while ((1 << order) < total_size && order <= MAX_ORDER) {
		order++;
	}
	return order;
}

// Allouer de la mémoire en utilisant le buddy allocator
void *kheap_alloc(size_t size) {
	if (!kernel_heap) return NULL;

	int order = find_order(size);
	if (order > MAX_ORDER) {
		qemu_printf("kheap_alloc: Taille demandée %ld dépasse la taille maximale de bloc.\n", size);
		return NULL;
	}

	// Trouver le plus petit bloc disponible
	int current_order = order;
	while (current_order <= MAX_ORDER && !kernel_heap->free_lists[current_order]) {
		current_order++;
	}

	if (current_order > MAX_ORDER) {
		qemu_printf("kheap_alloc: Aucun bloc approprié trouvé pour la taille %ld.\n", size);
		return NULL;
	}

	// Diviser les blocs jusqu'à atteindre l'ordre souhaité
	while (current_order > order) {
		split_block(current_order);
		current_order--;
	}

	// Allouer le bloc
	buddy_block_t *block = kernel_heap->free_lists[order];
	if (!block) {
		qemu_printf("kheap_alloc: Échec de l'allocation du bloc.\n");
		return NULL;
	}
	kernel_heap->free_lists[order] = block->next;
	block->is_free = false;

	// Calculer l'adresse de retour (après le bloc metadata)
	void *allocated_addr = (void *)((uintptr_t)block + sizeof(buddy_block_t));

	qemu_printf("kheap_alloc: Bloc de %ld bytes alloué à l'adresse 0x%p\n", block->size, allocated_addr);

	return allocated_addr;
}

// Libérer la mémoire en utilisant le buddy allocator
void kheap_free(void *p) {
	if (!p || !kernel_heap) return;

	// Calculer l'adresse du bloc
	buddy_block_t *block = (buddy_block_t *)((uintptr_t)p - sizeof(buddy_block_t));

	// Vérifier si le pointeur est dans les limites du heap
	if ((uintptr_t)block < HEAP_START || (uintptr_t)block >= (uintptr_t)(HEAP_START + kernel_heap->size)) {
		qemu_printf("kheap_free: Pointeur 0x%p hors des limites du heap.\n", p);
		return;
	}

	// Marquer le bloc comme libre
	block->is_free = true;

	// Déterminer l'ordre du bloc
	int order = 0;
	while ((1 << order) < block->size && order <= MAX_ORDER) {
		order++;
	}

	// Ajouter le bloc à la liste libre correspondante
	block->next = kernel_heap->free_lists[order];
	kernel_heap->free_lists[order] = block;

	qemu_printf("kheap_free: Bloc de %ld bytes libéré à l'adresse 0x%p\n", block->size, p);

	// Coalescer les buddies libres
	uintptr_t block_addr = (uintptr_t)block;
	size_t size = block->size;

	while (order < MAX_ORDER) {
		// Calculer l'adresse du buddy
		uintptr_t buddy_addr = block_addr ^ (1 << order);
		buddy_block_t *buddy = (buddy_block_t *)buddy_addr;

		// Vérifier si le buddy est libre et de la même taille
		bool buddy_free = false;
		buddy_block_t *current = kernel_heap->free_lists[order];
		buddy_block_t *prev = NULL;

		while (current) {
			if (current == buddy && current->is_free && current->size == size) {
				buddy_free = true;
				break;
			}
			prev = current;
			current = current->next;
		}

		if (!buddy_free) {
			break; // Aucun buddy libre à coalescer
		}

		// Retirer le buddy de la liste libre
		if (prev) {
			prev->next = current->next;
		} else {
			kernel_heap->free_lists[order] = current->next;
		}

		// Fusionner le bloc avec le buddy
		if (buddy_addr < block_addr) {
			block = buddy;
			block_addr = buddy_addr;
		}

		block->size *= 2;
		order++;
		qemu_printf("kheap_free: Blocs coalescés en un bloc de %ld bytes à l'adresse 0x%p\n", block->size, (void *)block_addr);
	}
}

// Initialiser le heap avec le buddy allocator
void initialize_heap(page_directory_t *dir) {
	if (kernel_heap) return;

	kernel_heap = &kernel_heap_struct;
	memset(kernel_heap, 0, sizeof(heap_t));

	kernel_heap->size = HEAP_INITIAL_SIZE;
	kernel_heap->dir = dir;

	// Initialiser le buddy allocator
	buddy_init(kernel_heap);

	qemu_printf("initialize_heap: Heap initialisé avec une taille de %ld bytes\n", HEAP_INITIAL_SIZE);
	printk("initialize_heap: Heap initialisé avec une taille de %ld bytes\n", HEAP_INITIAL_SIZE);
}

// Optionnel : Liste des blocs du heap pour le débogage
void list_heap_blocks(void) {
	if (!kernel_heap) {
		qemu_printf("Heap non initialisé.\n");
		return;
	}

	qemu_printf("===== Liste des Blocs du Heap =====\n");
	for (int i = 0; i <= MAX_ORDER; i++) {
		buddy_block_t *current = kernel_heap->free_lists[i];
		while (current) {
			qemu_printf("Bloc de %ld bytes à l'adresse 0x%p - %s\n",
						current->size,
						(void *)current,
						current->is_free ? "Libre" : "Alloué");
			current = current->next;
		}
	}
	qemu_printf("===================================\n");
}

/**
 * @brief Intermediate allocation function handling alignment and physical addresses.
 */
static void *intermediate_alloc(size_t size, uint8_t align, uint32_t *phys) {
	/* If the kernel heap is initialized, we use it */
	if (kernel_heap) {
		void *addr = kheap_alloc(size);
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
		/* Use ealloc to allocate memory without relying on the heap */
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
	if (!p) return kmalloc(size);

	size_t old_size = ksize(p);
	void *new_p = kmalloc(size);
	if (!new_p) return NULL;

	memcpy(new_p, p, old_size);
	kfree(p);
	return new_p;
}

size_t ksize(void *p) {
	if (!p) return 0;

	if (kernel_heap) {
		buddy_block_t *block = (buddy_block_t *)((uintptr_t)p - sizeof(buddy_block_t));
		return block->size - sizeof(buddy_block_t);
	} else {
	}
}