/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsod.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 20:57:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/20 14:47:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BSOD_H
#define BSOD_H

#include <kernel.h>

#define BSOD_MEMORY (uint16_t *)(__HIGHER_HALF_KERNEL__ == true ? (0xC00B8000) : (0x000B8000))
#define BSOD_VGA_ENTRY(uc, color) (color | uc)
#define BSOD_BACKGROUND_ENTRY(color) (((color)))
#define BSOD_LOGO_TEXT 0xF000
#define BSOD_TEXT 0x4F00
#define BSOD_BACKGROUND 0x4400

extern uint16_t *g_bsod_buffer;

extern void bsod(const char *error, const char *file);

#define __BSOD_BUFFER__ 64
extern char __bsod_error[__BSOD_BUFFER__];
extern char __bsod_file[__BSOD_BUFFER__];

#define __BSOD_UPDATE(e) __bsod_update_error(e, __FILE__)

static inline int __bsod_update_error(const char *error, const char *file)
{
    for (uint8_t i = 0; i < __BSOD_BUFFER__; ++i)
    {
        __bsod_error[i] = '\0';
        __bsod_file[i] = '\0';
    }
    for (uint8_t i = 0; error[i] != '\0'; ++i)
        __bsod_error[i] = error[i];
    for (uint8_t i = 0; file[i] != '\0'; ++i)
        __bsod_file[i] = file[i];
    return (0);
}

#endif /* !BSOD_H */