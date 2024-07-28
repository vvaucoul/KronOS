/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_vga.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 00:16:53 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 00:47:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>

#include <drivers/vga.h>
#include <system/pit.h>

static void clear_screen(void) {
	for (size_t y = 0; y < VGA_HEIGHT; ++y) {
		for (size_t x = 0; x < VGA_WIDTH; ++x) {
			const size_t index = y * VGA_WIDTH + x;
			__VGA_MEMORY[index] = VGA_ENTRY(' ', VGA_ENTRY_COLOR(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
		}
	}
}

void workflow_vga(void) {
	clear_screen();
	vga_disable_cursor();

	terminal_writestring_location("Warning: this test is not recommended for people with epilepsy.", VGA_WIDTH / 2 - (64 / 2), VGA_HEIGHT / 2);

	ksleep(3);

	clear_screen();

	// Write each character on each array index (0-255)
	for (uint8_t x = 0; x < VGA_WIDTH; ++x) {
		for (uint8_t c = 0; c < 255; ++c) {

			for (uint8_t y = 0; y < VGA_HEIGHT; ++y) {
				const uint32_t index = y * VGA_WIDTH + x;
				__VGA_MEMORY[index] = VGA_ENTRY(c, VGA_ENTRY_COLOR(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
			}
			busy_wait(1);
		}
	}

	ksleep(1);

	// Color background
	for (enum vga_color bg = VGA_COLOR_BLACK; bg < VGA_COLOR_WHITE; ++bg) {
		for (uint8_t y = 0; y < VGA_HEIGHT; ++y) {
			for (uint8_t x = 0; x < VGA_WIDTH; ++x) {
				const uint32_t index = y * VGA_WIDTH + x;
				__VGA_MEMORY[index] = VGA_ENTRY(' ', VGA_ENTRY_COLOR(VGA_COLOR_WHITE, bg));
			}
		}
		kmsleep(500);
	}

	ksleep(1);

	clear_screen();
}
