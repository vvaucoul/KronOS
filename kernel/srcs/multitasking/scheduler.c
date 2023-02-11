/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 23:11:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/scheduler.h>

#include <system/panic.h>

process_t *process_queue;
process_t *current_process;

void switch_task(t_regs *regs)
{
    memcpy(&current_process->regs, regs, sizeof(t_regs));
    if (current_process->next != NULL)
        current_process = current_process->next;
    else
        current_process = process_queue;
    memcpy(regs, &current_process->regs, sizeof(t_regs));
    switch_page_directory(current_process->page_directory);
}

void load_binary(uint8_t *data, uint32_t size)
{
    process_t *process = (process_t *)kmalloc(sizeof(process_t));

    printk("Alloc memory for process 0x%x\n", process);
    // process->process_page = create_user_page(UCODE_START, UCODE_START + size, current_directory);

    process->process_page = get_page(UCODE_START, current_directory);
    if (process->process_page == NULL)
        process->process_page = create_page(UCODE_START, current_directory);
    if (process->process_page == NULL)
        __PANIC("Could not get, create process page");
    printk("Create / GET user page\n");

    page_directory_t *tmp = current_directory;

    printk("Switch page directory\n");

    switch_page_directory(process->page_directory);
    kpause();
    memcpy((void *)UCODE_START, data, size);
    switch_page_directory(tmp);

    process->regs.eip = UCODE_START;
    process->regs.cs = 0x08;
    process->regs.ds = 0x10;
    process->regs.ss = 0x10;

    process->state = PROCESS_STATE_RUNNING;

    process_t *last_process = process_queue;

    if (last_process == NULL)
    {
        process_queue = process;
        return;
    }
    else
    {
        while (last_process->next != NULL)
            last_process = last_process->next;
        last_process->next = process;
    }
}