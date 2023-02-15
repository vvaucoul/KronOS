/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:13:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 15:04:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>

#define UCODE_START 0x600000

process_t process_table[MAX_PROCESS];

static uint32_t next_pid = 1;

void init_process(void)
{
    for (uint32_t i = 0; i < MAX_PROCESS; ++i)
    {
        process_table[i].state = PROCESS_STATE_UNUSED;
    }

    next_pid = 1;

    __LOG(__LOG_HEADER, "PROCESS", "Initialized");
}

/**
 * @brief Find a free slot in process table
    - Put it into initial state and return it
    - Return NULL if no free slot
 *
 * @return process_t*
 */
static process_t *alloc_new_process(uint32_t process_size)
{
    process_t *process = NULL;

    __LOG(__LOG_HEADER, "PROCESS", "Allocating new process");

    for (process = process_table; process < &process_table[MAX_PROCESS]; process++)
    {
        if (process->state == PROCESS_STATE_UNUSED)
            break;
    }

    if (process == NULL)
    {
        printk("No free slot in process table\n");
        return (NULL);
    }

    memset(process, 0, sizeof(process_t));
    process->state = PROCESS_STATE_INITING;

    __LOG(__LOG_HEADER, "PROCESS", "Found empty slot in process table");
    __LOG(__LOG_HEADER, "PROCESS", "Allocating process stack addr");

    process->stack = (uint32_t)kmalloc(PROCESS_STACK);
    assert(process->stack != 0);

    process->page_directory = kernel_directory;

    process->page = create_user_page(process->stack + UCODE_START, process->stack + UCODE_START + process_size, kernel_directory);

    if (process->page == NULL)
    {
        kfree((void *)process->stack);
        process->state = PROCESS_STATE_UNUSED;
        printk("Failed to allocate process page directory\n");
        return (NULL);
    }

    // Check if a page for the process already exists in the kernel directory
    page_t *page = get_page(process->stack, kernel_directory);
    if (page != NULL)
    {
        printk("Warning: process stack address already exists in kernel directory\n");
        printk("Page: 0x%x\n", page);

        printk("Process Directory: 0x%x - Page: 0x%x - Stack: 0x%x\n", process->page_directory, page, process->stack);
    }
    else
    {
        page = create_page(process->stack, kernel_directory);
        if (page == NULL)
        {
            kfree((void *)process->stack);
            destroy_user_page(process->page, kernel_directory);
            process->state = PROCESS_STATE_UNUSED;
            printk("Failed to allocate kernel page for process\n");
            return (NULL);
        }
    }

    __LOG(__LOG_HEADER, "PROCESS", "Allocated process page");

    process->pid = next_pid++;

    return (process);
}

void create_processus(void *entry_point, uint32_t size)
{
    __LOG(__LOG_HEADER, "PROCESS", "Creating init process");

    process_t *process = alloc_new_process(size);
    assert(process != NULL);

    __LOG(__LOG_HEADER, "PROCESS", "Init process created");

    strncpy(process->name, "init", sizeof(process->name) - 1);

    uint32_t sp = process->stack + PROCESS_STACK;
    sp -= sizeof(process_context_t);

    process->context = (process_context_t *)sp;
    memset(process->context, 0, sizeof(process_context_t));
    process->context->eip = (uint32_t)entry_point;

    process->context->esp = process->stack + PROCESS_STACK;
    process->context->ebp = process->stack + PROCESS_STACK;

    process->parent = NULL;
    process->child = NULL;

    process->state = PROCESS_STATE_READY;

    __LOG(__LOG_HEADER, "PROCESS", "Init process ready");

    printk("Init process created (pid %d)\n", process->pid);

    process->fn = entry_point;
}