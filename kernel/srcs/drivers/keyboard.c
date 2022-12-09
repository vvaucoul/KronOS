/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keyboard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:56:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 16:26:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/keyboard.h>
#include <shell/ksh.h>
#include <shell/ksh_termcaps.h>

bool __keyboard_uppercase = false;
kbd_lang_t __keyboard_lang = KBD_LANG_US;

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

unsigned char kbdfr[128] =
    {
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

static uint8_t __get_keyboard_lang(void)
{
    return (__keyboard_lang);
}

static unsigned char *__get_keyboard_codes(void)
{
    if (__keyboard_lang == KBD_LANG_US)
        return (kbdus);
    else
        return (kbdfr);
}

void set_keyboard_lang(kbd_lang_t lang)
{
    __keyboard_lang = lang;
}

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
        __keyboard_uppercase = true;
        return (true);
    }
    case KEYBOARD_CAPS:
    {
        __keyboard_uppercase = !__keyboard_uppercase;
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
    // printk("%d\n", scancode);

    /* If the top bit of the byte we read from the keyboard is
     *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
         *  shift, alt, or control keys... */
        // printk("Keyboard: Release %c\n", kbdus[scancode & 0x7F]);

        switch (scancode & 0x7F)
        {
        case KEYBOARD_LEFT_SHIFT:
        case KEYBOARD_RIGHT_SHIFT:
            __keyboard_uppercase = false;
        default:
            break;
        }
    }
    else
    {
        if ((scancode_handler(scancode)) == false)
        {
            if (isalpha(__get_keyboard_codes()[scancode]))
                ksh_write_char(__keyboard_uppercase == true ? __get_keyboard_codes()[scancode] - 32 : __get_keyboard_codes()[scancode]);
            else
                ksh_write_char(__get_keyboard_codes()[scancode]);
        }
    }
}

/* Installs the keyboard handler into IRQ1 */
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
    __UNUSED(__get_keyboard_lang);
}
