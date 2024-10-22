/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serial.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/01 16:14:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/19 11:14:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <system/serial.h>

#include <convert.h>
#include <macros.h>
#include <stdio.h>

// Définitions des drapeaux
#define PF_MINUS 0x01
#define PF_PLUS 0x02
#define PF_SPACE 0x04
#define PF_HASH 0x08
#define PF_ZERO 0x10

// Définitions des modificateurs de longueur
#define PL_NONE 0x00
#define PL_HH 0x01
#define PL_H 0x02
#define PL_L 0x04
#define PL_LL 0x08

static int __is_transmit_empty(void) {
	return (inportb(PORT_COM1 + 5) & 0x20);
}

static void __write_serial(const char c) {
	while (__is_transmit_empty() == 0)
		;
	outportb(PORT_COM1, c);
}

void serial_init(void) {
	outportb(PORT_COM1 + 1, 0x00); // Disable all interrupts
	outportb(PORT_COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
	outportb(PORT_COM1 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
	outportb(PORT_COM1 + 1, 0x00); //                  (hi byte)
	outportb(PORT_COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
	outportb(PORT_COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
	outportb(PORT_COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

// Fonction pour gérer les drapeaux et la largeur de champ
static int __qemu_printf_parse_flags(const char **fmt, int *width) {
	int flags = 0;
	*width = 0;

	while (**fmt) {
		switch (**fmt) {
			case '0': flags |= PF_ZERO; break;
			case '-': flags |= PF_MINUS; break;
			case '+': flags |= PF_PLUS; break;
			case ' ': flags |= PF_SPACE; break;
			case '#': flags |= PF_HASH; break;
			default:
				if (**fmt >= '0' && **fmt <= '9') {
					*width = *width * 10 + (**fmt - '0');
				} else {
					return flags;
				}
		}
		(*fmt)++;
	}
	return flags;
}

// Fonction pour gérer la longueur
static int __qemu_printf_parse_length(const char **fmt) {
	if (**fmt == 'l') {
		(*fmt)++;
		if (**fmt == 'l') {
			(*fmt)++;
			return PL_LL;
		}
		return PL_L;
	} else if (**fmt == 'h') {
		(*fmt)++;
		if (**fmt == 'h') {
			(*fmt)++;
			return PL_HH;
		}
		return PL_H;
	}
	return PL_NONE;
}

// Fonction pour écrire un nombre avec padding
static void __qemu_printf_write_num_padded(char *buffer, char *num_str, int width, int flags) {
	int len = strlen(num_str);
	int padding = width - len;

	if (flags & PF_MINUS) {
		strcpy(buffer, num_str);
		memset(buffer + len, ' ', padding);
	} else {
		if (flags & PF_ZERO) {
			memset(buffer, '0', padding);
		} else {
			memset(buffer, ' ', padding);
		}
		strcpy(buffer + padding, num_str);
	}
}

void qemu_printf(const char *format, ...) {
	char buffer[1024];
	char *str = buffer;
	const char *fmt = format;
	va_list args;
	va_start(args, format);

	while (*fmt) {
		if (*fmt == '%') {
			fmt++;

			// Gestion des drapeaux et de la largeur de champ
			int width;
			int flags = __qemu_printf_parse_flags(&fmt, &width);

			// Gestion de la longueur
			int length = __qemu_printf_parse_length(&fmt);

			switch (*fmt) {
				case 'c': {
					char c = (char)va_arg(args, int);
					*str++ = c;
					break;
				}
				case 's': {
					const char *s = va_arg(args, const char *);
					if (s == NULL)
						s = "(null)";

					if (width > 0) {
						char padded_str[1024];
						__qemu_printf_write_num_padded(padded_str, (char *)s, width, flags);
						strcpy(str, padded_str);
						str += strlen(padded_str);
					} else {
						while (*s) {
							*str++ = *s++;
						}
					}
					break;
				}
				case 'd':
				case 'i': {
					long long d;
					if (length == PL_LL) {
						d = va_arg(args, long long);
					} else if (length == PL_L) {
						d = va_arg(args, long);
					} else {
						d = va_arg(args, int);
					}

					char num_buffer[32];
					uitoa_base(d, 10, num_buffer);

					if (width > 0) {
						char padded_str[1024];
						__qemu_printf_write_num_padded(padded_str, num_buffer, width, flags);
						strcpy(str, padded_str);
						str += strlen(padded_str);
					} else {
						char *num_str = num_buffer;
						while (*num_str) {
							*str++ = *num_str++;
						}
					}
					break;
				}
				case 'u': {
					unsigned long long u;
					if (length == PL_LL) {
						u = va_arg(args, unsigned long long);
					} else if (length == PL_L) {
						u = va_arg(args, unsigned long);
					} else {
						u = va_arg(args, unsigned int);
					}

					char num_buffer[32];
					uitoa_base(u, 10, num_buffer);

					if (width > 0) {
						char padded_str[1024];
						__qemu_printf_write_num_padded(padded_str, num_buffer, width, flags);
						strcpy(str, padded_str);
						str += strlen(padded_str);
					} else {
						char *num_str = num_buffer;
						while (*num_str) {
							*str++ = *num_str++;
						}
					}
					break;
				}
				case 'x':
				case 'X': {
					unsigned long long x;
					if (length == PL_LL) {
						x = va_arg(args, unsigned long long);
					} else if (length == PL_L) {
						x = va_arg(args, unsigned long);
					} else {
						x = va_arg(args, unsigned int);
					}

					char num_buffer[32];
					uitoa_base(x, 16, num_buffer);

					if (*fmt == 'X') {
						// Convertir en majuscules si %X
						for (int i = 0; num_buffer[i]; i++) {
							if (num_buffer[i] >= 'a' && num_buffer[i] <= 'f') {
								num_buffer[i] -= 32;
							}
						}
					}

					if (width > 0) {
						char padded_str[1024];
						__qemu_printf_write_num_padded(padded_str, num_buffer, width, flags);
						strcpy(str, padded_str);
						str += strlen(padded_str);
					} else {
						char *num_str = num_buffer;
						while (*num_str) {
							*str++ = *num_str++;
						}
					}
					break;
				}
				case 'p': {
					void *p = va_arg(args, void *);
					uint32_t addr = (uint32_t)p;
					char num_buffer[32];
					uitoa_base(addr, 16, num_buffer);
					*str++ = '0';
					*str++ = 'x';
					char *num_str = num_buffer;
					while (*num_str) {
						*str++ = *num_str++;
					}
					break;
				}
				case '%':
					*str++ = '%';
					break;
				default:
					*str++ = '%';
					*str++ = *fmt;
					break;
			}
		} else {
			*str++ = *fmt;
		}
		fmt++;
	}
	*str = '\0';
	va_end(args);

	for (char *ptr = buffer; *ptr != '\0'; ptr++) {
		__write_serial(*ptr);
	}
}