/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:11:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 17:03:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KHEAP_H
#define KHEAP_H

#include <mm/mmu.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Définition des constantes */
#define HEAP_START 0xC0200000	   // Adresse de début du heap (exemple)
#define HEAP_INITIAL_SIZE 0x100000 // Taille initiale du heap : 1 MB
#define HEAP_MAX_SIZE 0x4000000	   // Taille maximale du heap : 64 MB

#define KERNEL_PAGE_DIR_INDEX 768 // 0xC0000000 / 0x400000

#define ALIGNMENT 16 // Alignement souhaité (peut être PAGE_SIZE si nécessaire)

#define ALIGN_UP(addr, align) (((uintptr_t)(addr) + ((align) - 1)) & ~((uintptr_t)((align) - 1)))
#define ALIGN_DOWN(addr, align) ((uintptr_t)(addr) & ~((uintptr_t)((align) - 1)))

/* Nombre maximal d'ordres pour le buddy allocator */
#define MAX_ORDER 12 // Supporte jusqu'à 4 MB (2^12 * 4 KB)

/* Magic Number pour l'intégrité des blocs du heap */
#define HEAP_BLOCK_MAGIC 0xDEADBEEF

/* Structure représentant un bloc buddy */
typedef struct buddy_block {
	size_t size;			  // Taille du bloc
	bool is_free;			  // Indicateur si le bloc est libre
	struct buddy_block *next; // Pointeur vers le prochain bloc libre de même ordre
} buddy_block_t;

/* Structure représentant le heap */
typedef struct heap {
	buddy_block_t *free_lists[MAX_ORDER + 1]; // Listes libres pour chaque ordre
	size_t size;							  // Taille actuelle du heap
	page_directory_t *dir;					  // Répertoire de pages
} heap_t;

/* Initialisation et gestion du heap */
void initialize_heap(page_directory_t *dir);
void list_heap_blocks(void);

/* Fonctions d'allocation */
void *kmalloc(size_t size);
void *kmalloc_a(size_t size);
void *kmalloc_p(size_t size, uint32_t *phys);
void *kmalloc_ap(size_t size, uint32_t *phys);

/* Fonctions de libération et de redimensionnement */
void kfree(void *p);
void *kcalloc(size_t num, size_t size);
void *krealloc(void *p, size_t size);

/* Fonction pour obtenir la taille allouée */
size_t ksize(void *p);

/* Fonctions supplémentaires pour la gestion virtuelle (si nécessaire) */
void *vmalloc(size_t size);
void vfree(void *p);
void *vcalloc(size_t num, size_t size);
void *vrealloc(void *p, size_t size);
size_t vsize(void *p);

#endif /* !KHEAP_H */