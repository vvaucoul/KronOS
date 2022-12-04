/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ksh_buffer.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/29 19:17:50 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:19:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KSH_BUFFER_H
#define KSH_BUFFER_H

#include <kernel.h>

#define __KSH_INPUT_BUFFER_SIZE__ (size_t)(128)

extern char ksh_line_buffer[__KSH_INPUT_BUFFER_SIZE__];
extern size_t ksh_line_index;

extern void ksh_buffer_init(void);
extern void ksh_update_buffer(const char *str, size_t length);
extern void ksh_buffer_add_char(char c);
extern void ksh_insert_char_at(char c, size_t index);
extern void ksh_buffer_delete_last_char(void);
extern void ksh_buffer_delete_char_at(size_t index);
extern void ksh_buffer_clear(void);

#endif /* KSH_BUFFER_H */