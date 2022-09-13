/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsod.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 20:57:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/13 21:35:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BSOD_H
# define BSOD_H

#include <kernel.h>

#define BSOD_MEMORY (uint16_t *)0x000B8000
#define BSOD_VGA_ENTRY(uc, color) (color | uc)
#define BSOD_BACKGROUND_ENTRY(color) (((color)))
#define BSOD_LOGO_TEXT 0xF000
#define BSOD_TEXT 0x4F00
#define BSOD_BACKGROUND 0x4400

extern uint16_t *g_bsod_buffer;

extern void bsod(const char *error, const char *file);

#endif /* !BSOD_H */