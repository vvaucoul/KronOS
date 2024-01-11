/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blocks.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 19:03:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 19:34:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/blocks.h>

BlockDevice *block_devices[MAX_BLOCK_DEVICES] = {0};

int register_block_device(BlockDevice *device) {
    for (uint8_t i = 0; i < MAX_BLOCK_DEVICES; i++) {
        if (block_devices[i] == NULL) {
            block_devices[i] = device;
            return (i);
        }
    }
    __THROW("Cannot register block device", 1);
}

int unregister_block_device(BlockDevice *device) {
    for (uint8_t i = 0; i < MAX_BLOCK_DEVICES; i++) {
        if (block_devices[i] == device) {
            block_devices[i] = NULL;
            return (i);
        }
    }
    __THROW("Cannot unregister block device", 1);
}

int block_read(BlockDevice *device, uint64_t block, uint64_t count, void *buf) {
    if (device == NULL || device->read == NULL) {
        __THROW("Cannot read from block device", 1);
    }

    return (device->read(device, block, count, buf));
}

int block_write(BlockDevice *device, uint64_t block, uint64_t count, void *buf) {
    if (device == NULL || device->write == NULL) {
        __THROW("Cannot write to block device", 1);
    }

    return (device->write(device, block, count, buf));
}
