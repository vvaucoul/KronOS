/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:56:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/30 13:26:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/drivers/keyboard.h"

#include "../../includes/shell/ksh.h"
#include "../../includes/shell/ksh_termcaps.h"

bool __keyboard_shift = false;

unsigned char kbdus[128] =
    {
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

static bool scancode_handler(unsigned char scancode)
{
    switch (scancode)
    {
    /* KSHELL MODE */
    case KEYBOARD_KEY_ESCAPE:
        poweroff();
        return (true);
    case KEYBOARD_KEY_BACK:
        ksh_del_one();
        return (true);
    case KEYBOARD_KEY_ARROW_LEFT:
        ksh_move_cursor_left();
        return (true);
    case KEYBOARD_KEY_ARROW_RIGHT:
        ksh_move_cursor_right();
        return (true);
    case KEYBOARD_KEY_ARROW_DOWN:
        ksh_move_cursor_down();
        return (true);
    case KEYBOARD_KEY_ARROW_TOP:
        ksh_move_cursor_up();
        return (true);
    case KEYBOARD_KEY_ENTER:
        ksh_new_line();
        return (true);
    case KEYBOARD_KEY_SUPPR:
        ksh_suppr_char();
        return (true);
    case KEYBOARD_F1:
        reboot();
        return (true);
    case KEYBOARD_LEFT_SHIFT:
    case KEYBOARD_RIGHT_SHIFT:
    {
        __keyboard_shift = true;
        return (true);
    }
    default:
        break;
    }
    return (false);
}

void keyboard_handler(struct regs *r)
{
    (void)r;
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inportb(0x60);
    // kprintf("%d\n", scancode);

    /* If the top bit of the byte we read from the keyboard is
     *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
         *  shift, alt, or control keys... */
        // kprintf("Keyboard: Release %c\n", kbdus[scancode & 0x7F]);

        switch (scancode & 0x7F)
        {
        case KEYBOARD_LEFT_SHIFT:
        case KEYBOARD_RIGHT_SHIFT:
            __keyboard_shift = false;
        default:
            break;
        }
    }
    else
    {
        if ((scancode_handler(scancode)) == false)
        {
            if (isalpha(kbdus[scancode]))
                ksh_write_char(__keyboard_shift == true ? kbdus[scancode] - 32 : kbdus[scancode]);
            else
                ksh_write_char(kbdus[scancode]);
        }
    }
}

/* Installs the keyboard handler into IRQ1 */
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
}
