/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acpi.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 12:37:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 12:38:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

/**
 * This file contains the declarations and definitions related to ACPI (Advanced Configuration and Power Interface).
 * ACPI is an open standard for power management and configuration of computer hardware.
 * This header file provides the necessary includes and defines for ACPI functionality.
 */

typedef struct {
	char Signature[4]; // "DSDT", "SSDT", etc.
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
} __attribute__((packed)) acpi_header_t;



#endif /* !ACPI_H */