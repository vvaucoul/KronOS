/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cache.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 20:19:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:06:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/fs/cache.h>
#include <system/fs/file.h>

#include <syscall/syscall.h>

#include <mm/mm.h>
#include <multitasking/process.h>

// cache_manager_t *init_cache_manager(uint32_t size) {
//     cache_manager_t *manager = (cache_manager_t *)kmalloc(sizeof(cache_manager_t));
//     manager->ht = hashtable_create(size);
//     return manager;
// }

// file_cache_t *get_or_create_cache(cache_manager_t *manager, const char *fs) {
//     file_cache_t *cache = hashtable_get(manager->ht, fs);
//     if (!cache) {
//         cache = (file_cache_t *)kmalloc(sizeof(file_cache_t));
//         cache->head = NULL;
//         hashtable_put(manager->ht, fs, cache);
//     }
//     return cache;
// }

// File *cache_open_file(cache_manager_t *manager, const char *fs, const char *path, int mode) {
//     file_cache_t *cache = get_or_create_cache(manager, fs);

//     cache_entry_t *entry = cache->head;
//     while (entry) {
//         if (strcmp(entry->path, path) == 0) {
//             entry->ref_count++;
//             return entry->file;
//         }
//         entry = entry->next;
//     }

//     int fd = sys_open(path, 0, mode); // Todo: Check for flags
//     if (fd == -1) {
//         return NULL;
//     }

//     task_t *proc = get_current_task();
//     File *file = proc->fd_table->files[fd];
//     entry = (cache_entry_t *)kmalloc(sizeof(cache_entry_t));
//     strncpy(entry->path, path, sizeof(entry->path));
//     entry->file = file;
//     entry->ref_count = 1;
//     entry->next = cache->head;
//     cache->head = entry;

//     return file;
// }

// int cache_close_file(cache_manager_t *manager, const char *fs, const char *path) {
//     file_cache_t *cache = get_or_create_cache(manager, fs);

//     cache_entry_t *entry = cache->head;
//     cache_entry_t *prev = NULL;

//     while (entry) {
//         if (strcmp(entry->path, path) == 0) {
//             entry->ref_count--;
//             if (entry->ref_count == 0) {
//                 if (prev) {
//                     prev->next = entry->next;
//                 } else {
//                     cache->head = entry->next;
//                 }

//                 task_t *proc = get_current_task();
//                 int result = close_file(proc, entry->file->fd);
//                 free(entry);
//                 return result;
//             }
//             return 0;
//         }
//         prev = entry;
//         entry = entry->next;
//     }

//     return -1;
// }

// void destroy_cache_manager(cache_manager_t *manager) {
//     // Iterate and free all caches
//     hashtable_iterator_t *it = hashtable_iterator(manager->ht);
//     const char *key;
//     while ((key = hashtable_next(it)) != NULL) {
//         file_cache_t *cache = hashtable_get(manager->ht, key);
//         cache_entry_t *entry = cache->head;
//         while (entry) {
//             cache_entry_t *next = entry->next;
//             kfree(entry);
//             entry = next;
//         }
//         kfree(cache);
//     }
//     hashtable_destroy(manager->ht);
//     kfree(manager);
// }

// Todo: create function get_file_cache with an hashtable

Hashtable *cache_manager = NULL;

// Initialiser le cache
static file_cache_t *init_file_cache() {
    file_cache_t *cache = (file_cache_t *)kmalloc(sizeof(file_cache_t));
    cache->head = NULL;
    return cache;
}

file_cache_t *get_file_cache(const char *fs) {
    if (cache_manager == NULL) {
        if ((cache_manager = hashtable_create(VFS_MAX_FS)) == NULL) {
            return NULL;
        }
    }
    if (hashtable_contains_key(cache_manager, fs) == 0) {

        file_cache_t *cache = init_file_cache();

        if (cache == NULL) {
            return NULL;
        }

        hashtable_insert(cache_manager, fs, cache);
        return (cache);
    } else {
        return (hashtable_get(cache_manager, fs));
    }
    return NULL;
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
int cache_close_file(file_cache_t *cache, int fd) {
    cache_entry_t *entry = cache->head;
    cache_entry_t *prev = NULL;

    // Rechercher l'entrée dans le cache
    while (entry) {
        if (entry->file->fd == fd) {
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

int cache_has_file_by_path(file_cache_t *cache, const char *path) {
    cache_entry_t *entry = cache->head;
    while (entry) {
        if (strcmp(entry->path, path) == 0) {
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

int cache_has_file_by_fd(file_cache_t *cache, int fd) {
    cache_entry_t *entry = cache->head;
    while (entry) {
        if (entry->file->fd == fd) {
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}