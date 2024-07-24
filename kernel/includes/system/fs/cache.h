/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cache.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 20:19:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 20:25:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CACHE_H
#define CACHE_H

#include <system/fs/file.h>

typedef struct cache_entry {
    char path[256];           // Chemin du fichier
    File *file;               // Pointeur vers la structure de fichier
    uint32_t ref_count;       // Compteur de références
    struct cache_entry *next; // Pointeur vers l'entrée suivante
} cache_entry_t;

typedef struct file_cache {
    cache_entry_t *head; // Pointeur vers la première entrée du cache
} file_cache_t;

// Déclarations des fonctions du cache
file_cache_t *init_file_cache();
void destroy_file_cache(file_cache_t *cache);
File *cache_open_file(file_cache_t *cache, const char *path, int mode);
int cache_close_file(file_cache_t *cache, const char *path);

#endif /* !CACHE_H */