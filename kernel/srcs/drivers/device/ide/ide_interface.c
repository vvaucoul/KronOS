/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ide_interface.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 09:43:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 10:15:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/devices.h>
#include <drivers/device/ide.h>

/**
 * @brief Read from an IDE device
 *
 * @param device IDE Device
 * @param lba Logical Block Address
 * @param sectors Number of sectors to read
 * @param buffer Buffer to read to
 *
 * @note This function is used to read from an IDE device
 *
 * @return int 0 if success, -1 if failed
 */
int ide_device_read(void *device, uint32_t lba, uint32_t sectors, void *buffer) {
    return (ide_read((IDEDevice *)device, lba, sectors, buffer));
}

/**
 * @brief Write to an IDE device
 *
 * @param device IDE Device
 * @param lba Logical Block Address
 * @param sectors Number of sectors to write
 * @param buffer Buffer to write from
 *
 * @note This function is used to write to an IDE device
 *
 * @return int 0 if success, -1 if failed
 */
int ide_device_write(void *device, uint32_t lba, uint32_t sectors, const void *buffer) {
    return (ide_write((IDEDevice *)device, lba, sectors, buffer));
}

/**
 * @brief Get the size of an IDE device
 *
 * @param device IDE Device
 *
 * @note This function is used to get the size of an IDE device
 *
 * @return uint32_t Size of the device
 */
uint32_t ide_device_get_size(void *device) {
    IDEDevice *ide_dev = (IDEDevice *)device;

    return (ide_dev->capabilities);
}

/**
 * @brief Get the sector size of an IDE device
 *
 * @param device IDE Device
 *
 * @note This function is used to get the sector size of an IDE device
 *
 * @return uint32_t Sector size of the device
 */
uint32_t ide_device_get_sector_size(void *device) {
    IDEDevice *ide_dev = (IDEDevice *)device;

    return (ide_dev->sector_size);
}

/**
 * @brief Get the sector count of an IDE device
 *
 * @param device IDE Device
 *
 * @note This function is used to get the sector count of an IDE device
 *
 * @return uint32_t Sector count of the device
 */
uint32_t ide_device_get_sector_count(void *device) {
    IDEDevice *ide_dev = (IDEDevice *)device;

    return (ide_dev->sector_count);
}
