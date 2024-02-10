/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:37:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 12:19:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
#define KERNEL_H

#pragma once

#include <hephaistos.h>

#define __KERNEL_VERSION__ "0.6.0"
#define __KERNEL_NAME__ "KronOS"

#define __DISPLAY_INIT_LOG__ true
#define __HIGHER_HALF_KERNEL__ false

extern void kernel_log_info(const char *part, const char *name);

extern int kmain(hex_t magic_number, hex_t addr, uint32_t *kstack);

// tmp
__attribute__((unused)) extern void test_user_function();
__attribute__((unused)) extern void switch_to_user_mode();

#define __THROW(msg, err, ...)                        \
    {                                                 \
        printk(_RED "WARNING: "                       \
                    "[%s:%u] \n\t\t- " msg _END "\n", \
               __FILE__, __LINE__, ##__VA_ARGS__);    \
        return (err);                                 \
    }

#define __THROW_NO_RETURN(msg, ...)                   \
    {                                                 \
        printk(_RED "WARNING: "                       \
                    "[%s:%u] \n\t\t- " msg _END "\n", \
               __FILE__, __LINE__, ##__VA_ARGS__);    \
        return;                                       \
    }

#define __WARN(msg, err, ...)                            \
    {                                                    \
        printk(_YELLOW "WARNING: "                       \
                       "[%s:%u] \n\t\t- " msg _END "\n", \
               __FILE__, __LINE__, ##__VA_ARGS__);       \
        return (err);                                    \
    }

#define __WARN_NO_RETURN(msg, ...)                       \
    {                                                    \
        printk(_YELLOW "WARNING: "                       \
                       "[%s:%u] \n\t\t- " msg _END "\n", \
               __FILE__, __LINE__, ##__VA_ARGS__);       \
        return;                                          \
    }

#define __WARND(msg, ...)                                \
    {                                                    \
        printk(_YELLOW "WARNING: "                       \
                       "[%s:%u] \n\t\t- " msg _END "\n", \
               __FILE__, __LINE__, ##__VA_ARGS__);       \
    }

#define __INFO(msg, err, ...)           \
    {                                   \
        printk(_END "\t\t- "   \
                       "" msg " "_END \
                       "\n",            \
               ##__VA_ARGS__);          \
        return (err);                   \
    }

#define __INFO_NO_RETURN(msg, ...)      \
    {                                   \
        printk(_END "\t\t- "   \
                       "" msg " "_END \
                       "\n",            \
               ##__VA_ARGS__);          \
        return;                         \
    }

#define __INFOD(msg, ...)               \
    {                                   \
        printk(_END "\t\t- "   \
                       "" msg " "_END \
                       "\n",            \
               ##__VA_ARGS__);          \
    }

#endif /* !KERNEL_H */