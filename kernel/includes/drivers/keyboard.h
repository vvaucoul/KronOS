/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:37:28 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/20 17:04:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <drivers/tty.h>
#include <kernel.h>
#include <system/io.h>
#include <system/irq.h>
#include <system/isr.h>

/* Import Keyboard keys */
#include "keyboard/k_keys.h"

#define KBD_BUFFER_SIZE 256
#define KBD_DATA_PORT 0x60

typedef enum e_kbd_lang {
	KEYBOARD_LAYOUT_FR,
	KEYBOARD_LAYOUT_EN
} kbd_lang_t;

extern void keyboard_handler(struct regs *r);
extern void keyboard_install(void);
extern void keyboard_set_layout(kbd_lang_t lang);
extern int getchar(void);

extern int kbd_uppercase_status(void);
extern int kbd_ctrl_status(void);
extern int kbd_alt_status(void);

#endif /* !KEYBOARD_H */