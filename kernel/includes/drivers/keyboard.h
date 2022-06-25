/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:37:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 16:20:06 by vvaucoul         ###   ########.fr       */
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
#define KEYBOARD_KEY_ENTER 28
#define KEYBOARD_KEY_SUPPR 83

#define KEYBOARD_KEY_ARROW_TOP 72
#define KEYBOARD_KEY_ARROW_LEFT 75
#define KEYBOARD_KEY_ARROW_RIGHT 77
#define KEYBOARD_KEY_ARROW_DOWN 80

#define KEYBOARD_SCREEN_F1 59
#define KEYBOARD_SCREEN_F2 60
#define KEYBOARD_SCREEN_F3 61
#define KEYBOARD_SCREEN_F4 62
#define KEYBOARD_SCREEN_F5 63
#define KEYBOARD_SCREEN_F6 64
#define KEYBOARD_SCREEN_F7 65
#define KEYBOARD_SCREEN_F8 66
#define KEYBOARD_SCREEN_F9 67
#define KEYBOARD_SCREEN_F10 68
#define KEYBOARD_SCREEN_F11 69
#define KEYBOARD_SCREEN_F12 70

extern void keyboard_handler(struct regs *r);
extern void keyboard_install();

#endif