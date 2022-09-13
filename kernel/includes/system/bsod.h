/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsod.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 20:57:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/11 21:03:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BSOD_H
# define BSOD_H

#include <kernel.h>

#define BSOD_MEMORY (uint16_t *)0x000B8000
#define BSOD_VGA_ENTRY(uc, color) (((uc) | ((color) << 8)))

extern uint16_t *g_bsod_buffer;

extern void bsod(char *msg);

#endif /* !BSOD_H */