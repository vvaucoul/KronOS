/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_heap.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 13:39:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 15:31:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>

#include <memory/kheap.h>

#include <kernel.h>
#include <system/panic.h>

/*******************************************************************************
 *                           KERNEL HEAP - WORKFLOW                            *
 ******************************************************************************/

void kheap_test(void)
{
    void *ptr = kmalloc(sizeof(char) * 15);

    if (ptr == NULL)
        __PANIC("Invalid PTR");
    else
    {
        kbzero(ptr, 15);
        kmemcpy(ptr, "Hello World !", 14);
        kprintf("PTR: [%s]\n", ptr);
    }

    void *ptr_2 = kmalloc(1024);
    
    if (ptr_2 == NULL)
        __PANIC("Invalid PTR");
    else
    {
        kbzero(ptr_2, 1024);
        kmemcpy(ptr_2, "42 !", 4);
        kprintf("PTR-2: [%s]\n", ptr_2);
        ((char *)(ptr_2))[2048] = 'A';
        kprintf("PTR-2 [2048] = %c\n", ((char *)ptr_2)[2048]);
    }
}