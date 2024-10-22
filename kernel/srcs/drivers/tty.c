/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tty.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 13:31:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/20 17:44:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/tty.h>
#include <system/io.h>

uint32_t tty_row = 0;
uint32_t tty_column = 0;
uint8_t tty_color = VGA_COLOR_LIGHT_GREY | VGA_COLOR_BLACK << 4;
uint16_t *tty_buffer = VGA_MEMORY;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t)uc | (uint16_t)color << 8;
}

static void tty_scroll(void) {
	// Copier chaque ligne vers la ligne précédente
	for (uint32_t y = 1; y < VGA_HEIGHT; y++) {
		for (uint32_t x = 0; x < VGA_WIDTH; x++) {
			const uint32_t index = y * VGA_WIDTH + x;
			const uint32_t prev_index = (y - 1) * VGA_WIDTH + x;
			tty_buffer[prev_index] = tty_buffer[index];
		}
	}
	// Nettoyer la dernière ligne
	for (uint32_t x = 0; x < VGA_WIDTH; x++) {
		const uint32_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
		tty_buffer[index] = vga_entry(' ', tty_color);
	}

	// Repositionner la ligne actuelle à la dernière ligne après défilement
	tty_row = VGA_HEIGHT - 1;
	tty_column = 0;
}

void tty_move_cursor(uint32_t x, uint32_t y) {
	const uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void tty_putentryat(char c, uint8_t color, uint32_t x, uint32_t y) {
	const uint32_t index = y * VGA_WIDTH + x;
	tty_buffer[index] = vga_entry(c, color);
}

void tty_putchar(char c) {
	switch (c) {
		case '\n':
			tty_column = 0;
			if (++tty_row == VGA_HEIGHT) {
				tty_scroll(); // Si on atteint la dernière ligne, on fait défiler
			}
			break;
		case '\r':
			tty_column = 0;
			break;
		case '\t':
			tty_column += 4;
			if (tty_column >= VGA_WIDTH) {
				tty_column = 0;
				if (++tty_row == VGA_HEIGHT) {
					tty_scroll(); // Faire défiler si nécessaire
				}
			}
			break;
		case '\b':
			if (tty_column > 0) {
				tty_column--;
			} else if (tty_row > 0) {
				tty_row--;
				tty_column = VGA_WIDTH - 1;
			}
			tty_putentryat(' ', tty_color, tty_column, tty_row);
			break;
		default:
			tty_putentryat(c, tty_color, tty_column, tty_row);
			if (++tty_column == VGA_WIDTH) {
				tty_column = 0;
				if (++tty_row == VGA_HEIGHT) {
					tty_scroll(); // Faire défiler si on dépasse la dernière ligne
				}
			}
			break;
	}
	tty_move_cursor(tty_column, tty_row);
}

// Imprimer une chaîne de caractères sur le terminal
void tty_write(const char *data, uint32_t size) {
	for (uint32_t i = 0; i < size; i++)
		tty_putchar(data[i]);
}

// Imprimer une chaîne de caractères avec une fonction simplifiée
void tty_writestring(const char *data) {
	tty_write(data, strlen(data));
}

// Fonction pour changer la couleur de texte
void tty_setcolor(uint8_t color) {
	tty_color = color;
}

void tty_set_background_color(uint8_t color) {
	tty_color = (tty_color & 0x0F) | (color << 4);
}

// Nettoyer l'écran
void tty_clear_screen() {
	for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
		for (uint32_t x = 0; x < VGA_WIDTH; x++) {
			const uint32_t index = y * VGA_WIDTH + x;
			tty_buffer[index] = vga_entry(' ', tty_color);
		}
	}
	tty_row = 0;
	tty_column = 0;
	tty_move_cursor(tty_column, tty_row);
}

void tty_move_cursor_up(void) {
	if (tty_row > 0) {
		tty_row--;
	}
	tty_move_cursor(tty_column, tty_row);
}

void tty_move_cursor_down(void) {
	if (tty_row < VGA_HEIGHT) {
		tty_row++;
	}
	tty_move_cursor(tty_column, tty_row);
}

void tty_move_cursor_left(void) {
	if (tty_column > 0) {
		tty_column--;
	}
	tty_move_cursor(tty_column, tty_row);
}

void tty_move_cursor_right(void) {
	if (tty_column < VGA_WIDTH) {
		tty_column++;
	}
	tty_move_cursor(tty_column, tty_row);
}

void tty_refresh_cursor(void) {
	tty_move_cursor(tty_column, tty_row);
}

uint16_t tty_get_char(uint32_t x, uint32_t y) {
	return tty_buffer[y * VGA_WIDTH + x];
}

void tty_set_char(uint32_t x, uint32_t y, uint16_t c) {
	tty_buffer[y * VGA_WIDTH + x] = c;
}

void tty_enable_cursor(void) {
	outb(0x3D4, 0x0A);					   // VGA_CRTC_INDEX, VGA_CURSOR_START_REG
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 14); // VGA_CRTC_DATA
	outb(0x3D4, 0x0B);					   // VGA_CRTC_INDEX, VGA_CURSOR_END_REG
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 15); // VGA_CRTC_DATA
}

void tty_disable_cursor(void) {
	outb(0x3D4, 0x0A); // VGA_CRTC_INDEX, VGA_CURSOR_START_REG
	outb(0x3D5, 0x20); // VGA_CRTC_DATA
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 INIT TTY DRIVER                                ||
// ! ||--------------------------------------------------------------------------------||

void tty_init(void) {
	tty_row = 0;
	tty_column = 0;
	tty_buffer = VGA_MEMORY;
	tty_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	tty_clear_screen();
}
