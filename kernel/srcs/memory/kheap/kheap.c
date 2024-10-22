/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 00:17:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 17:03:02 by vvaucoul         ###   ########.fr       */
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

/* Structure globale représentant le heap */
static heap_t *kernel_heap = NULL;
static heap_t kernel_heap_struct;

/* Fonction d'initialisation du buddy allocator */
static void buddy_init(heap_t *heap) {
	// Initialiser toutes les listes libres à NULL
	for (int i = 0; i <= MAX_ORDER; i++) {
		heap->free_lists[i] = NULL;
	}

	// Créer le bloc initial de taille maximale
	size_t initial_size = 1 << MAX_ORDER; // 4 MB
	buddy_block_t *initial = (buddy_block_t *)kmalloc(sizeof(buddy_block_t));
	if (!initial) {
		qemu_printf("buddy_init: Échec de l'allocation du bloc initial.\n");
		__PANIC("Échec de l'initialisation du buddy allocator");
	}
	initial->size = initial_size;
	initial->is_free = true;
	initial->next = NULL;

	// Ajouter le bloc initial à la liste libre correspondante
	heap->free_lists[MAX_ORDER] = initial;

	qemu_printf("buddy_init: Buddy allocator initialisé avec un bloc de taille %ld octets\n", initial_size);
}

/* Fonction pour trouver l'ordre approprié pour une taille donnée */
static int find_order(size_t size) {
	int order = 0;
	size_t total_size = size + sizeof(buddy_block_t);
	while ((1 << order) < total_size && order < MAX_ORDER) {
		order++;
	}
	return order;
}

/* Fonction pour diviser un bloc en deux buddies */
static void split_block(int current_order, heap_t *heap) {
	if (current_order <= 0 || current_order > MAX_ORDER) return;

	buddy_block_t *block = heap->free_lists[current_order];
	if (!block) return;

	// Retirer le bloc de la liste libre actuelle
	heap->free_lists[current_order] = block->next;

	// Diviser le bloc en deux buddies de taille inférieure
	size_t split_size = block->size / 2;

	buddy_block_t *buddy1 = block;
	buddy1->size = split_size;
	buddy1->is_free = true;
	buddy1->next = heap->free_lists[current_order - 1];
	heap->free_lists[current_order - 1] = buddy1;

	buddy_block_t *buddy2 = (buddy_block_t *)((uintptr_t)buddy1 + split_size);
	buddy2->size = split_size;
	buddy2->is_free = true;
	buddy2->next = heap->free_lists[current_order - 1];
	heap->free_lists[current_order - 1] = buddy2;

	qemu_printf("split_block: Divisé un bloc de taille %ld en deux blocs de taille %ld\n", block->size, split_size);
}

/* Fonction d'allocation utilisant le buddy allocator */
static void *buddy_alloc(size_t size, heap_t *heap) {
	int order = find_order(size);
	if (order > MAX_ORDER) {
		qemu_printf("buddy_alloc: La taille demandée %ld dépasse la taille maximale supportée.\n", size);
		return NULL;
	}

	// Trouver le plus petit bloc libre disponible
	int current_order = order;
	while (current_order <= MAX_ORDER && !heap->free_lists[current_order]) {
		current_order++;
	}

	if (current_order > MAX_ORDER) {
		qemu_printf("buddy_alloc: Aucun bloc libre disponible pour la taille %ld.\n", size);
		return NULL;
	}

	// Diviser les blocs jusqu'à atteindre l'ordre souhaité
	while (current_order > order) {
		split_block(current_order, heap);
		current_order--;
	}

	// Allouer le bloc
	buddy_block_t *block = heap->free_lists[current_order];
	heap->free_lists[current_order] = block->next;
	block->is_free = false;

	qemu_printf("buddy_alloc: Alloué un bloc de taille %ld à l'adresse 0x%p\n", block->size, (void *)block);

	// Retourner l'adresse utilisateur (après la structure buddy_block_t)
	return (void *)((uintptr_t)block + sizeof(buddy_block_t));
}

/* Fonction de libération utilisant le buddy allocator */
static void buddy_free(void *ptr, heap_t *heap) {
	if (!ptr) return;

	// Récupérer le bloc buddy à partir du pointeur utilisateur
	buddy_block_t *block = (buddy_block_t *)((uintptr_t)ptr - sizeof(buddy_block_t));
	block->is_free = true;

	// Déterminer l'ordre du bloc
	int order = 0;
	while ((1 << order) < block->size && order < MAX_ORDER) {
		order++;
	}

	// Chercher le buddy du bloc
	uintptr_t buddy_addr = (uintptr_t)block ^ (1 << order);
	buddy_block_t *buddy = (buddy_block_t *)buddy_addr;

	// Vérifier si le buddy est libre et de même taille
	if (buddy->is_free && buddy->size == block->size) {
		// Retirer le buddy de la liste libre
		buddy_block_t **current = &heap->free_lists[order];
		while (*current && *current != buddy) {
			current = &((*current)->next);
		}
		if (*current == buddy) {
			*current = buddy->next;
		}

		// Fusionner les deux buddies
		if (buddy < block) {
			buddy->size *= 2;
			block = buddy;
		} else {
			block->size *= 2;
		}

		// Ajouter le bloc fusionné à la liste libre supérieure
		block->next = heap->free_lists[order + 1];
		heap->free_lists[order + 1] = block;

		qemu_printf("buddy_free: Fusionné le bloc à 0x%p avec son buddy pour former un bloc de taille %ld\n", (void *)block, block->size);

		// Répéter la fusion récursivement
		buddy_free((void *)block, heap);
	} else {
		// Ajouter le bloc à la liste libre
		block->next = heap->free_lists[order];
		heap->free_lists[order] = block;

		qemu_printf("buddy_free: Libéré le bloc à 0x%p de taille %ld\n", (void *)block, block->size);
	}
}

/* Fonction de création et d'initialisation du heap */
static heap_t *create_heap(page_directory_t *dir, size_t size) {
	heap_t *heap = &kernel_heap_struct;
	memset(heap, 0, sizeof(heap_t));

	heap->size = 0;
	heap->dir = dir;

	// Initialiser le buddy allocator
	buddy_init(heap);

	qemu_printf("create_heap: Heap créé à l'adresse 0x%p avec une taille de %ld octets\n", (void *)HEAP_START, size);
	printk("create_heap: Heap créé à l'adresse 0x%p avec une taille de %ld octets\n", (void *)HEAP_START, size);

	return heap;
}

/* Fonction d'initialisation globale du heap */
void initialize_heap(page_directory_t *dir) {
	// Créer le heap
	kernel_heap = create_heap(dir, HEAP_INITIAL_SIZE);
	if (kernel_heap == NULL) {
		__PANIC("Heap initialization failed");
	}
	printk("initialize_heap: Heap initialisé avec succès\n");
	qemu_printf("initialize_heap: Heap initialisé avec succès\n");
}

/* Fonction d'allocation publique */
void *kmalloc(size_t size) {
	return buddy_alloc(size, kernel_heap);
}

/* Fonction de libération publique */
void kfree(void *p) {
	buddy_free(p, kernel_heap);
}

/* Fonctions de support pour l'allocation alignée et avec adresse physique */
void *kmalloc_a(size_t size) {
	void *ptr = buddy_alloc(size, kernel_heap);
	if (!ptr) return NULL;
	uintptr_t addr = (uintptr_t)ptr;
	uintptr_t aligned_addr = ALIGN_UP(addr, ALIGNMENT);
	return (void *)aligned_addr;
}

void *kmalloc_p(size_t size, uint32_t *phys) {
	void *ptr = buddy_alloc(size, kernel_heap);
	if (phys && ptr) {
		page_t *page = mmu_get_page((uintptr_t)ptr, mmu_get_kernel_directory());
		if (page)
			*phys = page->frame * PAGE_SIZE + ((uintptr_t)ptr & 0xFFF);
		else
			*phys = 0;
	}
	return ptr;
}

void *kmalloc_ap(size_t size, uint32_t *phys) {
	void *ptr = buddy_alloc(size, kernel_heap);
	if (phys && ptr) {
		page_t *page = mmu_get_page((uintptr_t)ptr, mmu_get_kernel_directory());
		if (page)
			*phys = page->frame * PAGE_SIZE + ((uintptr_t)ptr & 0xFFF);
		else
			*phys = 0;
	}
	return ptr;
}

/* Fonction de réallocation */
void *krealloc(void *p, size_t size) {
	if (!p) {
		return buddy_alloc(size, kernel_heap);
	}

	if (size == 0) {
		kfree(p);
		return NULL;
	}

	// Récupérer le bloc buddy à partir du pointeur utilisateur
	buddy_block_t *block = (buddy_block_t *)((uintptr_t)p - sizeof(buddy_block_t));
	size_t old_size = block->size;

	if (old_size >= size) {
		// Le bloc actuel est suffisant
		return p;
	} else {
		// Allouer un nouveau bloc
		void *new_ptr = buddy_alloc(size, kernel_heap);
		if (!new_ptr) return NULL;

		// Copier les données de l'ancien bloc vers le nouveau
		memcpy(new_ptr, p, old_size);

		// Libérer l'ancien bloc
		kfree(p);

		return new_ptr;
	}
}

/* Fonction de comptage de la taille allouée */
size_t ksize(void *p) {
	if (!p) return 0;
	buddy_block_t *block = (buddy_block_t *)((uintptr_t)p - sizeof(buddy_block_t));
	return block->size;
}

/* Fonction de liste des blocs du heap pour débogage */
void list_heap_blocks(void) {
	if (!kernel_heap) {
		qemu_printf("Heap non initialisé.\n");
		return;
	}

	qemu_printf("===== Blocs du Heap =====\n");
	for (int order = 0; order <= MAX_ORDER; order++) {
		buddy_block_t *current = kernel_heap->free_lists[order];
		while (current) {
			qemu_printf("Bloc à 0x%p - Taille: %ld octets - %s\n",
						(void *)current,
						current->size,
						current->is_free ? "Libre" : "Alloué");
			current = current->next;
		}
	}
	qemu_printf("=========================\n");
}
