/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 12:41:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/31 11:30:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/display.h>

static uint16_t detect_bios_area_hardware(void)
{
    const uint16_t bda_detected_hardware = 0x410; // Assuming the value is 0x410
    return bda_detected_hardware;
}

enum e_video_type get_bios_area_video_type(void)
{
    return (enum e_video_type)(detect_bios_area_hardware() & 0x30);
}