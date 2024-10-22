/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 16:55:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <stdbool.h> // bool
#include <stddef.h>	 // size_t
#include <stdint.h>	 // uint32_t

#include <mm/mmu.h> // page_directory_t

// Constants de base
#define HEAP_START 0xC0200000		// Adresse de départ du heap (exemple)
#define HEAP_INITIAL_SIZE (1 << 22) // Taille initiale du heap : 4 MB
#define HEAP_MAX_SIZE (1 << 26)		// Taille maximale du heap : 64 MB

#define ALIGNMENT 16 // Alignement souhaité

#define ALIGN_UP(addr, align) (((uintptr_t)(addr) + ((align) - 1)) & ~((uintptr_t)((align) - 1)))
#define ALIGN_DOWN(addr, align) ((uintptr_t)(addr) & ~((uintptr_t)((align) - 1)))

// Magic Number pour l'intégrité des blocs
#define HEAP_BLOCK_MAGIC 0xDEADBEEF

// Structure d'un bloc buddy
typedef struct buddy_block {
	size_t size;			  // Taille du bloc
	bool is_free;			  // Indique si le bloc est libre
	struct buddy_block *next; // Pointeur vers le bloc suivant dans la liste libre
} buddy_block_t;

// Structure du heap
typedef struct heap {
	buddy_block_t *free_lists[13]; // Listes libres pour les ordres 0 à 12 (2^0 à 2^12)
	size_t size;				   // Taille actuelle du heap
	page_directory_t *dir;		   // Répertoire de pages associé
} heap_t;

// void create_heap(uint32_t start, uint32_t initial_size, uint32_t max_size);
void initialize_heap(page_directory_t *dir);
void list_heap_blocks(void);

void *kmalloc(size_t size);
void *kmalloc_a(size_t size);
void *kmalloc_p(size_t size, uint32_t *phys);
void *kmalloc_ap(size_t size, uint32_t *phys);

void kfree(void *p);
void *kcalloc(size_t num, size_t size);
void *krealloc(void *p, size_t size);

size_t ksize(void *p);

void *vmalloc(size_t size);
void vfree(void *p);
void *vcalloc(size_t num, size_t size);
void *vrealloc(void *p, size_t size);

size_t vsize(void *p);

// bool heap_predicate(data_t a, data_t b);
// heap_array_t heap_array_create(void *addr, uint32_t max_size, heap_node_predicate_t predicate);
// void heap_array_insert_element(data_t data, heap_array_t *array);
// data_t heap_array_get_element(uint32_t index, heap_array_t *array);
// void heap_array_remove_element(uint32_t index, heap_array_t *array);
// void heap_destroy(heap_array_t *array);

#endif /* !KHEAP_H */