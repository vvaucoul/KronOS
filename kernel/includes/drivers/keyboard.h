/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:37:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 01:26:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <drivers/vga.h>
#include <kernel.h>
#include <system/io.h>
#include <system/irq.h>
#include <system/isr.h>

#define KEYBOARD_KEY_ESCAPE 1
#define KEYBOARD_KEY_BACK 14
#define KEYBOARD_KEY_ENTER 28
#define KEYBOARD_KEY_SUPPR 83

#define KEYBOARD_KEY_ARROW_TOP 72
#define KEYBOARD_KEY_ARROW_LEFT 75
#define KEYBOARD_KEY_ARROW_RIGHT 77
#define KEYBOARD_KEY_ARROW_DOWN 80

#define KEYBOARD_F1 59
#define KEYBOARD_LEFT_SHIFT 42
#define KEYBOARD_RIGHT_SHIFT 54
#define KEYBOARD_CAPS 58
#define KEYBOARD_LEFT_CTRL 29
#define KEYBOARD_RIGHT_CTRL 97
#define KEYBOARD_LEFT_ALT 56
#define KEYBOARD_RIGHT_ALT 100

typedef enum e_kbd_lang {
	KEYBOARD_LAYOUT_FR,
	KEYBOARD_LAYOUT_EN
} kbd_lang_t;

void keyboard_handler(struct regs *r);
void keyboard_install(void);
void keyboard_set_layout(kbd_lang_t lang);
int getchar(void);

#endif /* !KEYBOARD_H */