/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vesa.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 18:32:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/08 16:19:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VESA_H
# define VESA_H

# include <kernel.h>

/*
**  VBE = Video Bios Extension (VBE 2.0) (VESA)
**
**  - VBE 2.0 is a standard for video cards
**  - VESA = Video Electronics Standards Association
*/

typedef struct s_vesa
{

} vesa_t;

extern vesa_t vesa;

extern void vbe_enable_cursor(uint8_t start, uint8_t end);
extern void vbe_disable_cursor();
extern void vbe_update_cursor(uint8_t x, uint8_t y);
extern uint8_t vbe_get_cursor_position();

extern void init_vbe_mode(void);

#endif /* !VESA_H */