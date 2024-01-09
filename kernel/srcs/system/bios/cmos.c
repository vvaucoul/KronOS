/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmos.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:39:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cmos.h>
#include <system/io.h>

int32_t cmos_read(uint8_t addr)
{
    outb(CMOS_ADDR, addr);
    return inb(CMOS_DATA);
}
