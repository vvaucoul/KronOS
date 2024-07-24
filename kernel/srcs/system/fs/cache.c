/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cache.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 20:19:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 20:30:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/fs/cache.h>
#include <system/fs/file.h>

#include <multitasking/process.h>
#include <memory/memory.h>

// Initialiser le cache
file_cache_t *init_file_cache() {
    file_cache_t *cache = (file_cache_t *)kmalloc(sizeof(file_cache_t));
    cache->head = NULL;
    return cache;
}

// Détruire le cache
void destroy_file_cache(file_cache_t *cache) {
    cache_entry_t *entry = cache->head;
    while (entry) {
        cache_entry_t *next = entry->next;
        kfree(entry);
        entry = next;
    }
    kfree(cache);
}

// Ouvrir un fichier avec gestion du cache
File *cache_open_file(file_cache_t *cache, const char *path, int mode) {
    cache_entry_t *entry = cache->head;

    // Rechercher si le fichier est déjà dans le cache
    while (entry) {
        if (strcmp(entry->path, path) == 0) {
            entry->ref_count++;
            return entry->file;
        }
        entry = entry->next;
    }

    // Si le fichier n'est pas dans le cache, l'ouvrir et l'ajouter au cache
    int fd = allocate_file_descriptor(path, mode);
    if (fd == -1) {
        return NULL; // Erreur lors de l'ouverture du fichier
    }

    task_t *proc = get_current_task();
    File *file = proc->fd_table->files[fd];
    entry = (cache_entry_t *)kmalloc(sizeof(cache_entry_t));
    strncpy(entry->path, path, sizeof(entry->path));
    entry->file = file;
    entry->ref_count = 1;
    entry->next = cache->head;
    cache->head = entry;

    return file;
}

// Fermer un fichier avec gestion du cache
int cache_close_file(file_cache_t *cache, const char *path) {
    cache_entry_t *entry = cache->head;
    cache_entry_t *prev = NULL;

    // Rechercher l'entrée dans le cache
    while (entry) {
        if (strcmp(entry->path, path) == 0) {
            entry->ref_count--;
            if (entry->ref_count == 0) {
                // Si le compteur de références est zéro, supprimer l'entrée du cache
                if (prev) {
                    prev->next = entry->next;
                } else {
                    cache->head = entry->next;
                }
                int result = release_file_descriptor(entry->file->fd);
                kfree(entry);
                return result;
            }
            return 0;
        }
        prev = entry;
        entry = entry->next;
    }

    return -1; // Fichier non trouvé dans le cache
}