/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   backtrace.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 12:40:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/30 14:01:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/backtrace/backtrace.h>

backtrace_t backtrace[MAX_BACKTRACE];

static uint32_t __backtrace_idx = 0;

void init_backtrace(void)
{
    for (uint32_t i = 0; i < MAX_BACKTRACE; i++)
    {
        backtrace[i].addr = NULL;
        backtrace[i].line = 0;
        memset(backtrace[i].file, 0, MAX_BACKTRACE_FILE_NAME);
        memset(backtrace[i].func, 0, MAX_BACKTRACE_FUNC_NAME);
    }
}

void add_backtrace(void *addr, uint32_t line, char *file, char *func)
{
    backtrace[__backtrace_idx].addr = addr;
    backtrace[__backtrace_idx].line = line;
    memcpy(backtrace[__backtrace_idx].file, file, strlen(file));
    memcpy(backtrace[__backtrace_idx].func, func, strlen(func));
    __backtrace_idx++;
}

static void __move_offset(uint32_t idx)
{
    for (uint32_t i = idx; i < MAX_BACKTRACE; i++)
    {
        backtrace[i].addr = backtrace[i + 1].addr;
        backtrace[i].line = backtrace[i + 1].line;
        memcpy(backtrace[i].file, backtrace[i + 1].file, strlen(backtrace[i + 1].file));
        memcpy(backtrace[i].func, backtrace[i + 1].func, strlen(backtrace[i + 1].func));
    }
    backtrace[MAX_BACKTRACE - 1].addr = NULL;
    backtrace[MAX_BACKTRACE - 1].line = 0;
    memset(backtrace[MAX_BACKTRACE - 1].file, 0, MAX_BACKTRACE_FILE_NAME);
    memset(backtrace[MAX_BACKTRACE - 1].func, 0, MAX_BACKTRACE_FUNC_NAME);
}

void remove_backtrace(void *ptr)
{
    for (uint32_t i = 0; i < MAX_BACKTRACE; i++)
    {
        if (backtrace[i].addr == ptr)
        {
            backtrace[i].addr = NULL;
            backtrace[i].line = 0;
            memset(backtrace[i].file, 0, MAX_BACKTRACE_FILE_NAME);
            memset(backtrace[i].func, 0, MAX_BACKTRACE_FUNC_NAME);
            __move_offset(i);
            return;
        }
    }
}

void print_backtrace(void)
{
    for (uint32_t i = 0; i < MAX_BACKTRACE; i++)
    {
        if (backtrace[i].addr != NULL)
        {
            printk("0x%x: %s:%d %s\n", backtrace[i].addr, backtrace[i].file, backtrace[i].line, backtrace[i].func);
        }
    }
}