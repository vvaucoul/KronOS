/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cache.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 20:19:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 20:30:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CACHE_H
#define CACHE_H

#include <system/fs/file.h>

// typedef struct {
//     char path[256];
//     File *file;
//     uint32_t ref_count;
//     struct cache_entry *next;
// } cache_entry_t;

// typedef struct {
//     cache_entry_t *head;
// } file_cache_t;

// typedef struct {
//     Hashtable *ht; // Hashtable with keys as fs and values as file_cache_t*
// } cache_manager_t;

typedef struct cache_entry {
    char path[256];           // Chemin du fichier
    File *file;               // Pointeur vers la structure de fichier
    uint32_t ref_count;       // Compteur de références
    struct cache_entry *next; // Pointeur vers l'entrée suivante
} cache_entry_t;

typedef struct file_cache {
    cache_entry_t *head; // Pointeur vers la première entrée du cache
} file_cache_t;

extern Hashtable *cache_manager;

// Déclarations des fonctions du cache
// file_cache_t *init_file_cache();
extern file_cache_t *get_file_cache(const char *fs);
extern void destroy_file_cache(file_cache_t *cache);
extern File *cache_open_file(file_cache_t *cache, const char *path, int mode);
extern int cache_close_file(file_cache_t *cache, int fd);
extern int cache_has_file_by_path(file_cache_t *cache, const char *path);
extern int cache_has_file_by_fd(file_cache_t *cache, int fd);


#endif /* !CACHE_H */