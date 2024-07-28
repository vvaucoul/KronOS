/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsod.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 20:57:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 22:10:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <asm/asm.h>
#include <system/bsod.h>
#include <system/pit.h>
#include <system/irq.h>
#include <drivers/vga.h>

#include <macros.h>

uint16_t *g_bsod_buffer = BSOD_MEMORY;
char __bsod_error[__BSOD_BUFFER__] = {0};
char __bsod_file[__BSOD_BUFFER__] = {0};

volatile uint32_t g_bsod_lock = 0x0;

static void __bsod_background(void) {
    for (uint32_t i = 0; i < (VGA_WIDTH * VGA_HEIGHT); ++i)
        g_bsod_buffer[i] = BSOD_BACKGROUND_ENTRY(BSOD_BACKGROUND);
}

static void __bsod_write_string_at_location(const char *str, uint8_t x, uint8_t y, bool is_logo) {
    for (uint8_t i = 0; str[i] != '\0'; ++i)
        g_bsod_buffer[(y * VGA_WIDTH) + x + i] = BSOD_VGA_ENTRY(str[i], (is_logo ? BSOD_LOGO_TEXT : BSOD_TEXT));
}

static void __bsod_content(const char *error, const char *file) {
    const char *msgs[12] = {" KronOS ",
                            "An error has occurred.",
                            "Press Enter to restart the system.",
                            "Press F1 to continue.",
                            "Error: ",
                            "File: "};

    /* Write KronOS Logo */
    __bsod_write_string_at_location(msgs[0], (VGA_WIDTH / 2) - 5, (VGA_HEIGHT / 2) - 5, true);

    /* An error has occurred */
    __bsod_write_string_at_location(msgs[1], (VGA_WIDTH / 2) - 11, (VGA_HEIGHT / 2) - 3, false);

    /* Press Enter to restart the system */
    __bsod_write_string_at_location(msgs[2], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 3, false);

    /* Press F1 to continue */
    __bsod_write_string_at_location(msgs[3], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 4, false);

    /* Error: */
    __bsod_write_string_at_location(msgs[4], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 6, false);
    __bsod_write_string_at_location(error, (VGA_WIDTH / 2) - 20 + 7, (VGA_HEIGHT / 2) + 6, false);

    /* File: */
    __bsod_write_string_at_location(msgs[5], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 7, false);
    __bsod_write_string_at_location(file, (VGA_WIDTH / 2) - 20 + 6, (VGA_HEIGHT / 2) + 7, false);
}

void __bsod_callback(void) {
    if (g_bsod_lock > 0) {
        __bsod_write_string_at_location("System will reboot in ", 5, (VGA_HEIGHT)-2, false);
        __bsod_write_string_at_location((const char[]){(char)g_bsod_lock + 48, 0}, 27, (VGA_HEIGHT)-2, false);
        __bsod_write_string_at_location("s", 28, (VGA_HEIGHT)-2, false);
        g_bsod_lock--;
        ksleep(1);
        __bsod_callback();
    } else {
        reboot();
    }
}

void bsod(__unused__ const char *error, __unused__ const char *file) {
    __bsod_background();
    g_bsod_lock = 0x9;
    __bsod_content(__bsod_error, __bsod_file);
    __bsod_callback();

    /* Remove all IRQ */
    /* IRQ 0 still running */
    for (uint32_t i = 0; i < 16; ++i) {
        irq_uninstall_handler(i);
    }
}
