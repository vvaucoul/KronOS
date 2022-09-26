/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:46:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/14 01:13:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>

void *__request_new_page(size_t size)
{
}

void __pagination_init(void)
{
}

void __page_fault(struct regs *r)
{
    // __UNUSED__(r);
    /*
    US RW  P - Description
    0  0  0 - Supervisory process tried to read a non-present page entry
    0  0  1 - Supervisory process tried to read a page and caused a protection fault
    0  1  0 - Supervisory process tried to write to a non-present page entry
    0  1  1 - Supervisory process tried to write a page and caused a protection fault
    1  0  0 - User process tried to read a non-present page entry
    1  0  1 - User process tried to read a page and caused a protection fault
    1  1  0 - User process tried to write to a non-present page entry
    1  1  1 - User process tried to write a page and caused a protection fault
    */
    if (r->int_no & 0x1)
    {
        // User mode
        if (r->int_no & 0x2)
        {
            // Write
            __PANIC("Page fault: User mode write\n");
        }
        else
        {
            // Read
            __PANIC("Page fault: User mode read\n");
        }
    }
    else if (r->int_no & 0x2)
    {
        // Supervisor mode
        if (r->int_no & 0x2)
        {
            // Write
            __PANIC("Page fault: Supervisor mode write\n");
        }
        else
        {
            // Read
            __PANIC("Page fault: Supervisor mode read\n");
        }
    }
    else
    {
        // Kernel mode
        if (r->int_no & 0x2)
        {
            // Write
            __PANIC("Page fault: Kernel mode write\n");
        }
        else
        {
            // Read
            __PANIC("Page fault: Kernel mode read\n");
        }
    }
}