/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   page_fault.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/17 14:59:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/19 01:18:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/paging.h>
#include <system/panic.h>

void page_fault(struct regs *r)
{
    /*
     ** US RW  P - Description
     ** 0  0  0 - Supervisory process tried to read a non-present page entry
     ** 0  0  1 - Supervisory process tried to read a page and caused a protection fault
     ** 0  1  0 - Supervisory process tried to write to a non-present page entry
     ** 0  1  1 - Supervisory process tried to write a page and caused a protection fault
     ** 1  0  0 - User process tried to read a non-present page entry
     ** 1  0  1 - User process tried to read a page and caused a protection fault
     ** 1  1  0 - User process tried to write to a non-present page entry
     ** 1  1  1 - User process tried to write a page and caused a protection fault
     */

    uint32_t faulting_address;

    faulting_address = get_cr2();

    int present = !(r->err_code & 0x1);
    int rw = r->err_code & 0x2;
    int us = r->err_code & 0x4;
    int reserved = r->err_code & 0x8;
    int id = r->err_code & 0x10;

    kprintf(_RED "Page fault! "_END
                 "( ");
    if (present)
        kprintf("present ");
    if (rw)
        kprintf("read-only ");
    if (us)
        kprintf("user-mode ");
    if (reserved)
        kprintf("reserved ");
    if (id)
        kprintf("instruction fetch ");
    kprintf(") at ");
    kprintf(_RED "0x%08x"_END
                 "\n",
            faulting_address);
    kprintf("");
    __PANIC("Page fault");
}
