/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 12:41:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 11:30:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <kernel.h>

enum e_video_type
{
    VIDEO_TYPE_NONE = 0x00,
    VIDEO_TYPE_COLOUR = 0x20,
    VIDEO_TYPE_MONOCHROME = 0x30,
};

extern enum e_video_type get_bios_area_video_type(void);

#endif /* !DISPLAY_H */