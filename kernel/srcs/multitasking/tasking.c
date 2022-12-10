/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tasking.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/10 22:34:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 22:54:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/tasking.h>

#include <memory/paging.h>

static task_t *current_task = NULL;
static task_t main_task;
static task_t other_task;

static void new_main()
{
    printk("Hello Multitasking World !\n");
    yield();
}

void tasking_init(void)
{
    __asm__ volatile("movl %%cr3, %%eax; movl %%eax, %0;"
                     : "=m"(main_task.regs.cr3)::"%eax");
    __asm__ volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;"
                     : "=m"(main_task.regs.eflags)::"%eax");

    task_create(&other_task, new_main, main_task.regs.eflags, (uint32_t *)main_task.regs.cr3);
    main_task.next = &other_task;
    other_task.next = &main_task;

    current_task = &main_task;
}

void task_create(task_t *task, void (*entry)(void), uint32_t flags, uint32_t *page_directory)
{
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.esp = (uint32_t)create_page(0, current_directory) + PAGE_SIZE;
    task->regs.ebp = 0;
    task->regs.eip = (uint32_t)entry;
    task->regs.eflags = flags;
    task->regs.cr3 = (uint32_t)page_directory;
    task->next = NULL;
}

void yield()
{
    task_t *old = current_task;
    current_task = current_task->next;
    // task_switch(&old->regs, &current_task->regs);

    __UNUSED(old);
}