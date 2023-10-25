/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 09:43:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/25 12:46:03 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INITRD_H
#define INITRD_H

#include <filesystem/ext2/vfs_ext2.h>

typedef struct {
    uint32_t nfiles;
} initrd_header_t;

typedef struct {
    uint8_t magic;
    int8_t name[64];
    uint32_t offset;
    uint32_t length;
} initrd_file_header_t;

extern fs_node_t *initialise_initrd(uint32_t location);
extern void read_disk(void);

#endif /* INITRD_H */