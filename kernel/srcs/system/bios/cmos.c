/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmos.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:39:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/11 12:41:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cmos.h>
#include <system/io.h>

int32_t cmos_read(uint8_t addr)
{
    outb(CMOS_ADDR, addr);
    return inb(CMOS_DATA);
}
