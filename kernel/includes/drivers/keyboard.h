/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:37:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/24 00:51:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYBOARD_H
# define KEYBOARD_H

#include "../kernel.h"
#include "../terminal.h"
#include "../system/io.h"
#include "../system/isr.h"
#include "../system/irq.h"

#define KEYBOARD_KEY_ESCAPE 1
#define KEYBOARD_KEY_BACK 14

#define KEYBOARD_KEY_ARROW_TOP 72
#define KEYBOARD_KEY_ARROW_LEFT 75
#define KEYBOARD_KEY_ARROW_RIGHT 77
#define KEYBOARD_KEY_ARROW_DOWN 80

extern void keyboard_handler(struct regs *r);
extern void keyboard_install();

#endif