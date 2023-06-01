/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   backtrace.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 12:38:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/30 12:57:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BACKTRACE_H
#define BACKTRACE_H

#include <kernel.h>

#define MAX_BACKTRACE 1024

#define MAX_BACKTRACE_FILE_NAME 256
#define MAX_BACKTRACE_FUNC_NAME 256

typedef struct s_backtrace
{
    void *addr;
    char file[MAX_BACKTRACE_FILE_NAME];
    char func[MAX_BACKTRACE_FUNC_NAME];
    uint32_t line;
} backtrace_t;

extern backtrace_t backtrace[MAX_BACKTRACE];

extern void init_backtrace(void);
extern void add_backtrace(void *addr, uint32_t line, char *file, char *func);
extern void remove_backtrace(void *ptr);
extern void print_backtrace(void);

#define BACKTRACE() add_backtrace(__builtin_return_address(0), __FILE__, __func__)

#endif /* BACKTRACE_H */