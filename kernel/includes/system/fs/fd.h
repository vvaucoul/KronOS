/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 23:56:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 20:17:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FD_H
#define FD_H

// Define struct file to avoid circular dependencies
// struct file -> file.h
struct file;

#define MAX_FDS 256 // Nombre maximum de fichiers ouverts par processus

typedef struct file_descriptor_table {
    struct file *files[MAX_FDS]; // Table de descripteurs de fichiers
} fd_table_t;

extern fd_table_t *fd_table_init(void);
extern int allocate_file_descriptor(const char *path, int mode);
extern int release_file_descriptor(int fd);

#endif /* !FD_H */