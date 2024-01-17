/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   devices.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 17:15:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 21:51:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEVICES_H
#define DEVICES_H

#include <filesystem/vfs.h>
#include <kernel.h>

/**
 * @file devices.h
 * @brief This file contains the driver context for devices.
 *
 * This file contains the driver context for devices.
 */

#define MAX_DEVICES 32

typedef enum e_device_type {
    DEVICE_UNKNOWN,
    DEVICE_CHAR,
    DEVICE_BLOCK
} DeviceType;

typedef struct s_device Device;

typedef int (*DeviceRead)(Device *device, uint32_t sector, uint32_t count, void *buffer);
typedef int (*DeviceWrite)(Device *device, uint32_t sector, uint32_t count, const void *buffer);

typedef struct s_device {
    // Device informations
    char *name;
    uint32_t uid;
    DeviceType type;

    // Virtual File System
    Vfs *vfs;

    // Device functions
    DeviceRead read;
    DeviceWrite write;

    // Device size
    uint32_t sectors;
    uint32_t sector_size;

    // Device specific data
    void *device;
} Device;

extern Device *devices;
extern uint8_t devices_count;

extern void devices_init(void);

extern int device_register(Device *device);
extern int device_unregister(Device *device);

extern Device *device_get(uint32_t index);
extern Device *device_get_by_id(uint32_t uid);
extern Device *device_get_by_name(char *name);
extern uint32_t device_get_devices_count(void);

extern Device *device_init_new_device(char *name, DeviceType type, DeviceRead read, DeviceWrite write, void *device);

#endif /* !DEVICES_H */