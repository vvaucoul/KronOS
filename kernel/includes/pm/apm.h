/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 12:38:35 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 12:39:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APM_H
#define APM_H

#include <stdint.h>

/**
 * This file contains the declarations for APM related functions and structures.
 * APM provides advanced power management features for the system.
 */

typedef struct apm_table {
	uint16_t version;
	uint16_t cseg;
	uint32_t offset;
	uint16_t cseg_16;
	uint16_t dseg;
	uint16_t flags;
	uint16_t cseg_len;
	uint16_t cseg_16_len;
	uint16_t dseg_len;
} __attribute__((packed)) apm_table_t;

#endif /* !APM_H */