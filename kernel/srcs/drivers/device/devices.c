/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   devices.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 17:13:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:06:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/devices.h>
#include <mm/mm.h>

Device *devices = NULL;
uint8_t devices_count = 0;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                DEVICES FUNCTIONS                               ||
// ! ||--------------------------------------------------------------------------------||

void devices_init(void) {
    if (!(devices = kmalloc(sizeof(Device *) * MAX_DEVICES))) {
        __THROW_NO_RETURN("Cannot allocate memory for devices");
    } else {
        memset(devices, 0, sizeof(Device *) * MAX_DEVICES);
        devices_count = 0;
    }
}

int device_register(Device *device) {
    if (devices_count >= MAX_DEVICES) {
        __THROW("Cannot register device, too many devices", -1);
    }
    devices[devices_count] = *device;
    ++devices_count;
    return (0);
}

int device_unregister(Device *device) {
    for (uint32_t i = 0; i < devices_count; ++i) {
        if (devices[i].uid == device->uid) {
            devices[i] = devices[devices_count - 1];
            --devices_count;
            return (devices_count + 1);
        }
    }
    __THROW("Cannot unregister device, no device with this id", 1);
}

Device *device_init_new_device(char *name, DeviceType type, DeviceRead read, DeviceWrite write, DeviceSRead sread, DeviceSWrite swrite, DeviceInterface interface, void *device) {
    Device *new_device = NULL;

    if (!(new_device = kmalloc(sizeof(Device)))) {
        __THROW("Cannot allocate memory for new device", NULL);
    }
    memset(new_device, 0, sizeof(Device));

    new_device->name = name;
    new_device->type = type;
    new_device->read = read;
    new_device->write = write;
    new_device->sread = sread;
    new_device->swrite = swrite;
    new_device->device = device;
    new_device->interface = interface;

    new_device->uid = devices_count;
    new_device->vfs = NULL;

    return (new_device);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  DEVICE - GET                                  ||
// ! ||--------------------------------------------------------------------------------||

Device *device_get(uint32_t index) {
    if (index >= devices_count) {
        __THROW("Cannot get device, index out of range", NULL);
    }
    return (&devices[index]);
}

Device *device_get_by_id(uint32_t uid) {
    for (uint32_t i = 0; i < devices_count; ++i) {
        if (devices[i].uid == uid) {
            return (&devices[i]);
        }
    }
    __THROW("Cannot get device, no device with this id", NULL);
}

Device *device_get_by_name(char *name) {
    for (uint32_t i = 0; i < devices_count; ++i) {
        if (strcmp(devices[i].name, name) == 0) {
            return (&devices[i]);
        }
    }
    __THROW("Cannot get device, no device with this name", NULL);
}

uint32_t device_get_devices_count(void) {
    return (devices_count);
}