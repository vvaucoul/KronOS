/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:56:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/27 16:46:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/keyboard.h>
#include <shell/ksh.h>
#include <system/irq.h>
#include <shell/ksh_termcaps.h>

static bool __keyboard_uppercase = false;
static kbd_lang_t __keyboard_lang = KEYBOARD_LAYOUT_EN;

static unsigned char kbdus[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b',
    '\t',
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', KEYBOARD_LEFT_SHIFT,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', KEYBOARD_RIGHT_SHIFT,
    '*',
    0,   /* Alt */
    ' ', /* Space bar */
    0,   /* Caps lock */
    0,   /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, /* < ... F10 */
    0, /* 69 - Num lock*/
    0, /* Scroll Lock */
    0, /* Home key */
    0, /* Up Arrow */
    0, /* Page Up */
    '-',
    0, /* Left Arrow */
    0,
    0, /* Right Arrow */
    '+',
    0, /* 79 - End key*/
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0,
    0, /* F11 Key */
    0, /* F12 Key */
    0, /* All other keys are undefined */
};

static unsigned char kbdfr[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', ')', '=', '\b',
    '\t',
    'a', 'z', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',
    0,
    'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm',
    '%', '*', KEYBOARD_LEFT_SHIFT,
    '<', 'w', 'x', 'c', 'v', 'b', 'n',
    ',', ';', ':', '!', KEYBOARD_RIGHT_SHIFT,
    0,
    0,   /* Alt */
    ' ', /* Space bar */
    0,   /* Caps lock */
    0,   /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, /* < ... F10 */
    0, /* 69 - Num lock*/
    0, /* Scroll Lock */
    0, /* Home key */
    0, /* Up Arrow */
    0, /* Page Up */
    '-',
    0, /* Left Arrow */
    0,
    0, /* Right Arrow */
    '+',
    0, /* 79 - End key*/
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0,
    0, /* F11 Key */
    0, /* F12 Key */
    0, /* All other keys are undefined */
};

static unsigned char *get_keyboard_codes(void) {
    switch (__keyboard_lang) {
        case KEYBOARD_LAYOUT_FR:
            return kbdfr;
        case KEYBOARD_LAYOUT_EN:
        default:
            return kbdus;
    }
}

void keyboard_set_layout(kbd_lang_t lang) {
    __keyboard_lang = lang;
    printk("\t   Keyboard layout set to "_GREEN "%s"_END "\n",
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
        default:
            break;
    }
    return false;
}

volatile char lastKey = 0;
volatile bool keyReceived = false;

int getchar(void) {
    keyReceived = false;
    while (!keyReceived)
        ;
    return lastKey;
}

void keyboard_handler(struct regs *r) {
    (void)r;
    unsigned char scancode = inportb(0x60);

    if (scancode & 0x80) {
        char key = get_keyboard_codes()[scancode & 0x7F];
        if (key) {
            lastKey = key;
            keyReceived = true;
        }

        switch (scancode & 0x7F) {
            case KEYBOARD_LEFT_SHIFT:
            case KEYBOARD_RIGHT_SHIFT:
                __keyboard_uppercase = false;
                break;
            default:
                break;
        }
    } else {
        if (!scancode_handler(scancode)) {
            char key = get_keyboard_codes()[scancode];
            if (isalpha(key))
                ksh_write_char(__keyboard_uppercase ? key - 32 : key);
            else
                ksh_write_char(key);
        }
    }
}

void keyboard_install(void) {
    irq_install_handler(IRQ_KBD, keyboard_handler);
}