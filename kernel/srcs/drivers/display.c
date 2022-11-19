/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 12:41:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 12:46:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/display.h>

static uint16_t detect_bios_area_hardware(void)
{
    const uint16_t *bda_detected_hardware_ptr = (const uint16_t *)0x410;
    return (*bda_detected_hardware_ptr);
}

enum e_video_type get_bios_area_video_type(void)
{
    return (enum e_video_type)(detect_bios_area_hardware() & 0x30);
}