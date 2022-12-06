/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 18:32:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 14:14:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _STRING_H
#define _STRING_H

#include "../stddef/stddef.h"
#include "../stdbool/stdbool.h"

extern size_t kstrlen(const char *str);
extern size_t knbrlen(const int nbr);
extern size_t knbr_base_len(const uint32_t nbr, const uint8_t base);
extern int kstrcmp(const char *s1, const char *s2);
extern int kstrncmp(const char *s1, const char *s2, size_t length);
extern char *kstrtrunc(char *str, size_t length);
extern char *kstrcat(char *dest, const char *src);
extern char *kstrncat(char *dest, const char *src, size_t n);
extern char *kstrclr(char *new_str, char *str);

extern bool kccof(const char c, const char *str);
extern bool kacof(const char *str, const char *chars);

extern void *kmemset(void *ptr, int value, size_t length);
extern void *kbzero(void *ptr, size_t len);
extern void *kmemcpy(void *dst, const void *src, size_t length);
extern void *kmemmove(void *dst, const void *src, size_t length);
extern void *kmemchr(const void *s, int c, size_t n);
extern char *kstrrchr(const char *str, int n);
extern int kmemcmp(const void *s1, const void *s2, size_t n);
extern void *kmemjoin(void *s1, const void *s2, size_t n1, size_t n2);

#endif /* _STRING_H */