/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 00:07:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/scheduler.h>

#include <system/time.h>
#include <system/tss.h>

extern task_t *current_task;
extern task_t *ready_queue;

bool scheduler_initialized = false;

void init_scheduler(void) {
    scheduler_initialized = true;
}

void switch_task(void) {
    if (!scheduler_initialized)
        return;

    outportb(0x20, 0x20); // Send EOI to PIC

    uint32_t esp, ebp, eip;

    /* If we haven't initialised tasking yet, just return */
    if (!current_task)
        return;

    /* Read esp, ebp now for saving later on */
    __asm__ __volatile__("mov %%esp, %0"
                         : "=r"(esp));
    __asm__ __volatile__("mov %%ebp, %0"
                         : "=r"(ebp));

    eip = read_eip();

    /* Have we just switched tasks? */
    if (eip == 0x12345)
        return;

    /* Check if the current task has received a signal */
    __signal_handler(current_task);

    // printk("Task %d: %u\n", current_task->pid, get_cpu_load(current_task));
 
    // Just before we switch away from the current task, update its cpu_time
    // current_task->cpu_load.load_time += get_system_time() - current_task->cpu_load.last_start_time;

    /* No, we didn't switch tasks. Let's save some register values and switch */
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;

    /* Get the next task to run */
    current_task = current_task->next;
    /* If we fell off the end of the linked list start again at the beginning */
    if (!current_task)
        current_task = ready_queue;

    // Just after we've switched to the new task, update its start_time
    // current_task->cpu_load.last_start_time = get_system_time();
    // printk("Task %d: %u\n", current_task->pid, get_cpu_load(current_task));

    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;

    /* Make sure the memory manager knows we've changed page directory */
    current_directory = current_task->page_directory;

    /* Check if the current task can be woken up */
    // Todo: Check if task is Zombie - Stopped - Waiting etc...

    // if (current_task && current_task->pid > 1) {
    //     uint64_t current_time;
    //     tm_t time = gettime();
    //     current_time = mktime(&time);
    //     current_task->cpu_load.load_time += current_task->cpu_load.start_time - current_time;

    //     printk("Task %d: %u\n", current_task->pid, get_cpu_load(current_task));
    // }

    /* Change kernel stack over */
    tss_set_stack_pointer(current_task->kernel_stack + KERNEL_STACK_SIZE);

    __asm__ __volatile__("		\
    cli;			\
	mov %0, %%ecx;		\
	mov %1, %%esp;		\
	mov %2, %%ebp;		\
	mov %3, %%cr3;		\
	mov $0x12345, %%eax;	\
	sti;			\
	jmp *%%ecx		"
                         :
                         : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physicalAddr));
}
