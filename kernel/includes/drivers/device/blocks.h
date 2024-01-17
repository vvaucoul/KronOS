/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blocks.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:02:53 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 18:25:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCKS_H
#define BLOCKS_H

#include <kernel.h>

/**
 * @file blocks.h
 * @brief Header file for the blocks module in the filesystem context.
 *
 * This file contains the declarations and definitions for the blocks module,
 * which is responsible for managing blocks in the filesystem.
 */

#define MAX_BLOCK_DEVICES 16

typedef struct block_device {
    const char *name;
    void *device;

    uint32_t sectors;
    uint32_t sector_size;

    int (*read)(struct block_device *device, uint32_t sector, uint32_t count, void *buffer);
    int (*write)(struct block_device *device, uint32_t sector, uint32_t count, const void *buffer);
} __attribute__((packed)) BlockDevice;

extern BlockDevice *block_devices[MAX_BLOCK_DEVICES];

extern int register_block_device(BlockDevice *device);
extern int unregister_block_device(BlockDevice *device);

extern int block_read(BlockDevice *device, uint32_t sector, uint32_t count, void *buffer);
extern int block_write(BlockDevice *device, uint32_t sector, uint32_t count, const void *buffer);

#endif /* !BLOCKS_H */