/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vbe.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 18:32:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/18 19:54:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VBE_H
# define VBE_H

# include <kernel.h>

/*
**  VBE = Video Bios Extension (VBE 2.0) (VESA)
**
**  - VBE 2.0 is a standard for video cards
**  - VESA = Video Electronics Standards Association
*/

typedef struct s_vbe
{

} vbe_t;

extern vbe_t vbe;

extern void vbe_enable_cursor(uint8_t start, uint8_t end);
extern void vbe_disable_cursor();
extern void vbe_update_cursor(uint8_t x, uint8_t y);
extern uint8_t vbe_get_cursor_position();

extern void init_vbe_mode(void);

#endif /* !VBE_H */