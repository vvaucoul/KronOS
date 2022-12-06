/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/18 15:40:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 14:15:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>

void workflow_header(const char *function_name)
{
    const uint32_t len = strlen(function_name);
    const uint32_t len_line = 80;
    const uint32_t len_line_start = (len_line - len) / 2 - 1;
    const uint32_t len_line_end = len_line - len - len_line_start - 2;

    printk("\n"_GREEN);
    for (uint32_t i = 0; i < len_line_start; ++i)
        putchar(WORKFLOW_CHAR);
    printk(_END " %s "_GREEN, function_name);
    for (uint32_t i = 0; i < len_line_end; ++i)
        putchar(WORKFLOW_CHAR);
    printk(_END "\n\n");
}

void workflow_footer(void)
{
    printk(_GREEN "\n");
    for (uint32_t i = 0; i < 80; ++i)
        putchar(WORKFLOW_CHAR);
    printk(_END "\n\n");
}