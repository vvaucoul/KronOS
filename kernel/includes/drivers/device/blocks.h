/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blocks.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:02:53 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 19:30:51 by vvaucoul         ###   ########.fr       */
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
    void *device;

    uint32_t block_size;
    uint64_t block_count;

    int (*read)(struct block_device *device, uint64_t block, uint64_t count, void *buf);
    int (*write)(struct block_device *device, uint64_t block, uint64_t count, void *buf);
} __attribute__((packed)) BlockDevice;

extern BlockDevice *block_devices[MAX_BLOCK_DEVICES];

extern int register_block_device(BlockDevice *device);
extern int unregister_block_device(BlockDevice *device);

extern int block_read(BlockDevice *device, uint64_t block, uint64_t count, void *buf);
extern int block_write(BlockDevice *device, uint64_t block, uint64_t count, void *buf);

#endif /* !BLOCKS_H */