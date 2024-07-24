/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   devices.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 17:15:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEVICES_H
#define DEVICES_H

#include <fs/vfs/vfs.h>
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

typedef int (*DeviceRead)(void *device, uint32_t lba, uint32_t sectors, void *buffer);
typedef int (*DeviceWrite)(void *device, uint32_t lba, uint32_t sectors, const void *buffer);
typedef int (*DeviceSRead)(void *device, uint32_t offset, uint32_t size, void *buffer);
typedef int (*DeviceSWrite)(void *device, uint32_t offset, uint32_t size, const void *buffer);

typedef uint32_t (*DeviceSize)(void *device);
typedef uint32_t (*DeviceSectorSize)(void *device);
typedef uint32_t (*DeviceSectorCount)(void *device);

typedef struct s_device_interface {
    DeviceSize get_size;
    DeviceSectorSize get_sector_size;
    DeviceSectorCount get_sector_count;
} DeviceInterface;

typedef struct s_device {
    // Device informations
    char *name;
    uint32_t uid;
    DeviceType type;

    // Virtual File System Related to the device
    Vfs *vfs;

    // Device functions
    DeviceRead read;
    DeviceWrite write;
    DeviceSRead sread;
    DeviceSWrite swrite;

    // Interface functions
    DeviceInterface interface;

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

extern Device *device_init_new_device(char *name, DeviceType type, DeviceRead read, DeviceWrite write, DeviceSRead sread ,DeviceSWrite swrite, DeviceInterface interface, void *device);

#endif /* !DEVICES_H */