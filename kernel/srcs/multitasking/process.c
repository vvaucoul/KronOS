/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:13:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/12 19:16:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>

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
static process_t *alloc_new_process(void)
{
    process_t *process;
    bool found = false;

    __LOG(__LOG_HEADER, "PROCESS", "Allocating new process");

    for (process = process_table; process < &process_table[MAX_PROCESS]; process++)
    {
        if (process->state == PROCESS_STATE_UNUSED)
        {
            found = true;
            break;
        }
    }

    if (found == false)
    {
        printk("No free slot in process table\n");
        return (NULL);
    }
    else
    {
        process->state = PROCESS_STATE_INITING;

        __LOG(__LOG_HEADER, "PROCESS", "Found empty slot in process table");
        __LOG(__LOG_HEADER, "PROCESS", "Allocating process stack addr");

        process->stack = (uint32_t)kmalloc(PROCESS_STACK);
        assert(process->stack != 0);

        if (!(process->page_directory = get_page(process->stack, kernel_directory)))
            process->page_directory = create_page(process->stack, kernel_directory);

        assert(process->page_directory != NULL);

        __LOG(__LOG_HEADER, "PROCESS", "Allocated process page");

        process->pid = next_pid++;

        return (process);
    }
}

void create_processus(void *entry_point, uint32_t stack)
{
    __LOG(__LOG_HEADER, "PROCESS", "Creating init process");

    process_t *process = alloc_new_process();
    assert(process != NULL);

    __LOG(__LOG_HEADER, "PROCESS", "Init process created");

    strncpy(process->name, "init", sizeof(process->name) - 1);

    uint32_t sp = process->stack + PROCESS_STACK;
    sp -= sizeof(process_context_t);

    process->context = (process_context_t *)sp;
    memset(process->context, 0, sizeof(process_context_t));
    process->context->eip = (uint32_t)entry_point;

    process->context->esp = stack;
    process->context->ebp = stack;

    process->state = PROCESS_STATE_READY;

    __LOG(__LOG_HEADER, "PROCESS", "Init process ready");

    printk("Init process created (pid %d)\n", process->pid);

    process->fn = entry_point;
}