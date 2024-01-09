/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   page_fault.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:59:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/paging.h>
#include <memory/memory.h>
#include <system/panic.h>

void page_fault(struct regs *r)
{
    uint32_t faulting_address;

    faulting_address = get_cr2();

    int present = !(r->err_code & 0x1);
    int rw = r->err_code & 0x2;
    int us = r->err_code & 0x4;
    int reserved = r->err_code & 0x8;
    int id = r->err_code & 0x10;

    printk(_RED "Page fault! "_END
                "( ");
    if (present)
        printk("present ");
    if (rw)
        printk("read-only ");
    if (us)
        printk("user-mode ");
    if (reserved)
        printk("reserved ");
    if (id)
        printk("instruction fetch ");
    printk(") at ");
    printk(_RED "0x%08x"_END
                "\n",
           faulting_address);
    printk("");

    if (faulting_address == 0x0)
    {
        __PANIC("Page fault at NULL pointer");
    }
    else if (faulting_address >= KERNEL_BASE)
    {
        __PANIC("Page fault at kernel address");
    }
    else if (reserved)
    {
        __PANIC("Page fault at reserved address");
    }
    else if (rw && !present)
    {
        __PANIC("Page fault trying to write to non-present page");
    }
    else if (!us && present)
    {
        __PANIC("Page fault trying to execute kernel code");
    }
    else if (rw && us && present)
    {
        __PANIC("Page fault trying to write to read-only page");
    }
    else
    {
        __PANIC("Page fault");
    }
}
