/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:37:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/06 23:42:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYBOARD_H
#define KEYBOARD_H

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

#define KEYBOARD_F1 59
#define KEYBOARD_F2 60
#define KEYBOARD_F3 61
#define KEYBOARD_F4 62
#define KEYBOARD_F5 63
#define KEYBOARD_F6 64
#define KEYBOARD_F7 65
#define KEYBOARD_F8 66
#define KEYBOARD_F9 67
#define KEYBOARD_F10 68
#define KEYBOARD_F11 69
#define KEYBOARD_F12 70

#define KEYBOARD_LEFT_SHIFT 42
#define KEYBOARD_RIGHT_SHIFT 54
#define KEYBOARD_CAPS 58

typedef enum e_kbd_lang
{
    KBD_LANG_FR,
    KBD_LANG_US
} kbd_lang_t;

extern kbd_lang_t __keyboard_lang;

extern void keyboard_handler(struct regs *r);
extern void keyboard_install();
extern void set_keyboard_lang(kbd_lang_t lang);

extern bool __keyboard_uppercase;

#endif