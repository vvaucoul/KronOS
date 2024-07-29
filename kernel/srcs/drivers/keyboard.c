/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:56:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 01:28:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/keyboard.h>
#include <shell/ksh.h>
#include <shell/ksh_termcaps.h>
#include <system/irq.h>

#include <ctype.h>

static bool __keyboard_uppercase = false;
static bool __keyboard_ctrl = false;
static bool __keyboard_alt = false;
static kbd_lang_t __keyboard_lang = KEYBOARD_LAYOUT_EN;

static const unsigned char kbdus[128] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
	'9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', KEYBOARD_LEFT_SHIFT,
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEYBOARD_RIGHT_SHIFT,
	'*', 0, ' ', KEYBOARD_CAPS, KEYBOARD_F1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
	0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const unsigned char kbdfr[128] = {
	0, 27, '&', 0xE9, '"', '\'', '(', '-', 0xE8, '_',
	0xE7, 0xE0, ')', '=', '\b',
	'\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n', 0,
	'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 0xF9, '*', KEYBOARD_LEFT_SHIFT,
	'<', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', KEYBOARD_RIGHT_SHIFT,
	0, 0, ' ', KEYBOARD_CAPS, KEYBOARD_F1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
	0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const unsigned char *get_keyboard_codes(void) {
	return (__keyboard_lang == KEYBOARD_LAYOUT_FR) ? kbdfr : kbdus;
}

/**
 * Sets the keyboard layout to the specified language.
 *
 * @param lang The language to set the keyboard layout to.
 */
void keyboard_set_layout(kbd_lang_t lang) {
	__keyboard_lang = lang;
	printk("\t   Keyboard layout set to "_GREEN
		   "%s"_END
		   "\n",
		   lang == KEYBOARD_LAYOUT_EN ? "EN" : "FR");
}

static bool scancode_handler(unsigned char scancode) {
	if (!ksh_is_running())
		return false;

	switch (scancode) {
	case KEYBOARD_KEY_ESCAPE:
		poweroff();
		return true;
	case KEYBOARD_KEY_BACK:
		ksh_del_one();
		return true;
	case KEYBOARD_KEY_ARROW_LEFT:
		ksh_move_cursor_left();
		return true;
	case KEYBOARD_KEY_ARROW_RIGHT:
		ksh_move_cursor_right();
		return true;
	case KEYBOARD_KEY_ARROW_DOWN:
		ksh_move_cursor_down();
		return true;
	case KEYBOARD_KEY_ARROW_TOP:
		ksh_move_cursor_up();
		return true;
	case KEYBOARD_KEY_ENTER:
		ksh_new_line();
		return true;
	case KEYBOARD_KEY_SUPPR:
		ksh_suppr_char();
		return true;
	case KEYBOARD_F1:
		reboot();
		return true;
	case KEYBOARD_LEFT_SHIFT:
	case KEYBOARD_RIGHT_SHIFT:
		__keyboard_uppercase = true;
		return true;
	case KEYBOARD_CAPS:
		__keyboard_uppercase = !__keyboard_uppercase;
		return true;
	case KEYBOARD_LEFT_CTRL:
		__keyboard_ctrl = true;
		return true;
	case KEYBOARD_RIGHT_CTRL:
		__keyboard_ctrl = true;
		return true;
	case KEYBOARD_LEFT_ALT:
		__keyboard_alt = true;
		return true;
	case KEYBOARD_RIGHT_ALT:
		__keyboard_alt = true;
		return true;
	default:
		break;
	}
	return false;
}

volatile char lastKey = 0;
volatile bool keyReceived = false;

/**
 * @brief Retrieves a character from the keyboard input buffer.
 *
 * This function reads a character from the keyboard input buffer and returns it.
 *
 * @return The character read from the keyboard input buffer.
 */
int getchar(void) {
	keyReceived = false;
	while (!keyReceived)
		;
	return lastKey;
}

/**
 * @brief Handles keyboard interrupts.
 *
 * This function is the interrupt handler for keyboard interrupts. It takes a pointer to a struct regs
 * as a parameter, which contains the register values at the time of the interrupt. The function is responsible
 * for handling the keyboard interrupt and performing any necessary actions.
 *
 * @param r A pointer to a struct regs containing the register values at the time of the interrupt.
 */
void keyboard_handler(struct regs *r) {
	(void)r;
	unsigned char scancode = inportb(0x60);

	if (scancode & 0x80) {
		unsigned char keycode = scancode & 0x7F;
		char key = get_keyboard_codes()[keycode];
		if (key) {
			lastKey = key;
			keyReceived = true;
		}
		switch (keycode) {
		case KEYBOARD_LEFT_SHIFT:
		case KEYBOARD_RIGHT_SHIFT:
			__keyboard_uppercase = false;
			break;
		case KEYBOARD_LEFT_CTRL:
			__keyboard_ctrl = false;
			break;
		case KEYBOARD_RIGHT_CTRL:
			__keyboard_ctrl = false;
			break;
		case KEYBOARD_LEFT_ALT:
			__keyboard_alt = false;
			break;
		case KEYBOARD_RIGHT_ALT:
			__keyboard_alt = false;
			break;
		default:
			break;
		}
	} else {
		if (!scancode_handler(scancode)) {
			char key = get_keyboard_codes()[scancode];
			if (isalpha(key)) {
				ksh_write_char(__keyboard_uppercase ? toupper(key) : key);
			} else {
				ksh_write_char(key);
			}
		}
	}
	pic8259_send_eoi(IRQ_KBD);
}

/**
 * Installs the keyboard driver.
 */
void keyboard_install(void) {
	irq_install_handler(IRQ_KBD, keyboard_handler);
}