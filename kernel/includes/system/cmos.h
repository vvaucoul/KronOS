/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmos.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:40:14 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/11 12:44:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CMOS_H
#define CMOS_H

#include <kernel.h>

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

/* Convert BCD to binary */
/* BCD: Binary Coded Decimal */
#define BCD_TO_BIN(val) ((val) = ((val) & 15) + ((val) >> 4) * 10)

extern int32_t cmos_read(uint8_t addr);

#endif /* !CMOS_H */