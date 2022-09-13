/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsod.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 20:57:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/13 21:31:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/bsod.h>

uint16_t *g_bsod_buffer = BSOD_MEMORY;

static void __bsod_background(void)
{
    for (uint32_t i = 0; i < (VGA_WIDTH * VGA_HEIGHT); ++i)
        g_bsod_buffer[i] = BSOD_BACKGROUND_ENTRY(BSOD_BACKGROUND);
}

static void __bsod_write_string_at_location(const char *str, uint8_t x, uint8_t y, bool is_logo)
{
    for (uint8_t i = 0; str[i] != '\0'; ++i)
        g_bsod_buffer[(y * VGA_WIDTH) + x + i] = BSOD_VGA_ENTRY(str[i], (is_logo ? BSOD_LOGO_TEXT : BSOD_TEXT));
}

static void __bsod_content(const char *error, const char *file)
{
    const char *msgs[12] = {" KronOS ",
                            "An error has occurred.",
                            "Press Enter to restart the system.",
                            "Press F1 to continue.",
                            "Error: ",
                            "File: "};
    __bsod_write_string_at_location(msgs[0], (VGA_WIDTH / 2) - 5, (VGA_HEIGHT / 2) - 5, true);
    __bsod_write_string_at_location(msgs[1], (VGA_WIDTH / 2) - 11, (VGA_HEIGHT / 2) - 3, false);
    __bsod_write_string_at_location(msgs[2], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 3, false);
    __bsod_write_string_at_location(msgs[3], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 4, false);

    __bsod_write_string_at_location(msgs[4], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 6, false);
    __bsod_write_string_at_location(error, (VGA_WIDTH / 2) - 20 + 7, (VGA_HEIGHT / 2) + 6, false);

    __bsod_write_string_at_location(msgs[5], (VGA_WIDTH / 2) - 20, (VGA_HEIGHT / 2) + 7, false);
    __bsod_write_string_at_location(file, (VGA_WIDTH / 2) - 20 + 6, (VGA_HEIGHT / 2) + 7, false);
}

void bsod(const char *error, const char *file)
{
    __bsod_background();
    __bsod_content(error, file);
}
