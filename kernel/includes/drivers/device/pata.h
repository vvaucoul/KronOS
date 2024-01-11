/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pata.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 14:32:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/10 19:17:18 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATA_H
#define PATA_H

#include <kernel.h>

#define PATA_DRIVER 0

/**
 * @file pata.h
 * @brief Header file for the PATA (Parallel ATA) driver.
 *
 * This file contains the declarations and definitions for the PATA driver,
 * which provides functionality for interacting with Parallel ATA devices.
 */

typedef struct {
    char model[41];         // Model name
    char serial[21];        // Serial number
    uint16_t sectors[256];  // Sector data
    uint16_t capabilities;  // Capabilities
    uint16_t config;        // Configuration
    uint32_t size;          // Size in sectors
    uint16_t major_version; // Major version number
    uint16_t minor_version; // Minor version number
} __attribute__((packed)) PATAIdentify;

typedef struct {
    uint16_t io_base;
    uint16_t ctrl_base;
    PATAIdentify *id;
} PATADevice;

extern PATADevice *pata_dev;

extern int pata_init(uint16_t io_base, uint16_t ctrl_base);

extern uint8_t pata_identify(PATADevice *dev);
extern void pata_identify_device(PATADevice *dev);

extern int pata_read(PATADevice *dev, uint32_t lba, uint8_t *buffer, uint32_t sectors);
extern int pata_write(PATADevice *dev, uint32_t lba, const uint8_t *buffer, uint32_t sectors);

#endif /* !PATA_H */
