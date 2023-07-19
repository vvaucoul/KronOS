/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:37:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 18:18:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
#define KERNEL_H

#pragma once

#include <hephaistos.h>

#define __KERNEL_VERSION__ "0.5.0"
#define __KERNEL_NAME__ "KronOS"

#define __DISPLAY_INIT_LOG__ true
#define __HIGHER_HALF_KERNEL__ false

extern void kernel_log_info(const char *part, const char *name);

// tmp
__attribute__((unused)) extern void test_user_function();
__attribute__((unused)) extern void switch_to_user_mode();

#define __THROW(msg, err, ...)                     \
    {                                              \
        printk(_RED "WARNING: "                    \
                    "[%s:%u] " msg _END "\n",      \
               __FILE__, __LINE__, ##__VA_ARGS__); \
        return (err);                              \
    }

#define __THROW_NO_RETURN(msg, ...)                \
    {                                              \
        printk(_RED "WARNING: "                    \
                    "[%s:%u] " msg _END "\n",      \
               __FILE__, __LINE__, ##__VA_ARGS__); \
        return;                                    \
    }

#endif /* !KERNEL_H */