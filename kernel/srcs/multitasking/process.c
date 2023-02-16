/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:13:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/16 23:40:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>

#define USER_CODE_OFFSET 0x600000

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

    process->kernel_stack = (uint32_t)kmalloc(PROCESS_KERNEL_STACK);
    process->user_stack = (uint32_t)kmalloc(PROCESS_USER_STACK);

    assert(process->kernel_stack != 0);
    assert(process->user_stack != 0);

    process->page_directory = kernel_directory;

    process->page = create_user_page(process->user_stack + USER_CODE_OFFSET, process->user_stack + USER_CODE_OFFSET + process_size, kernel_directory);

    if (process->page == NULL)
    {
        kfree((void *)process->user_stack);
        kfree((void *)process->kernel_stack);

        process->state = PROCESS_STATE_UNUSED;

        printk("Failed to allocate process page directory\n");
        return (NULL);
    }

    page_t *page = get_page(process->user_stack, kernel_directory);
    if (page != NULL)
    {
        printk("Warning: process stack address already exists in kernel directory\n");
        printk("Page: 0x%x\n", page);

        printk("Process Directory: 0x%x - Page: 0x%x - Stack: 0x%x\n", process->page_directory, page, process->user_stack);
    }
    else
    {
        page = create_page(process->user_stack, kernel_directory);
        if (page == NULL)
        {
            kfree((void *)process->user_stack);
            kfree((void *)process->kernel_stack);

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

/**
 * @brief Create a processus object
    - Allocate a new process
    - Copy the name
    - Set the stack pointer
    - Set the context
    - Set the entry point
    - Set the level
    - Set the size
    - Return the process
 *
 * @param name
 * @param cpu_state
 * @param kernel_stack
 * @param entry_point
 * @param level
 * @param size
 * @return
 */
process_t *create_processus(const char *name, struct regs *cpu_state, void *kernel_stack, void (*entry_point)(void), process_level_t level, uint32_t size)
{
    process_t *process = NULL;

    __LOG(__LOG_HEADER, "PROCESS", "Creating init process");
    process = alloc_new_process(size);
    assert(process != NULL);

    __LOG(__LOG_HEADER, "PROCESS", "Init process created");

    memcpy(process->name, name, strlen(name));

    uint32_t __context = process->user_stack + PROCESS_USER_STACK - sizeof(struct regs);

    process->context = (struct regs *)__context;

    memset(process->context, 0, sizeof(struct regs));

    process->kernel_esp = process->kernel_stack + PROCESS_KERNEL_STACK;
    process->user_esp = process->user_stack + PROCESS_USER_STACK;

    process->context->esp = process->user_esp;

    process->context->eflags = cpu_state->eflags;

    process->context->ds = cpu_state->ds;
    process->context->es = cpu_state->es;
    process->context->fs = cpu_state->fs;
    process->context->gs = cpu_state->gs;

    process->context->cs = PROCESS_KERNEL_CODE_SELECTOR;
    process->context->ds = PROCESS_KERNEL_DATA_SELECTOR;
    process->context->ss = PROCESS_KERNEL_STACK_SELECTOR;

    process->context->eip = (uint32_t)entry_point;

    process->parent = NULL;
    process->child = NULL;

    process->state = PROCESS_STATE_READY;

    __LOG(__LOG_HEADER, "PROCESS", "Init process ready");

    printk("Init process created (pid %d)\n", process->pid);
}

/**
 * @brief Destroy a processus object
    - Free the stack
    - Free the page directory
    - Clear the context
    - Clear the name
    - Set the state to unused
 *
 * @param process
 */
void destroy_processus(process_t *process)
{
    __LOG(__LOG_HEADER, "PROCESS", "Destroying process");

    if (process->state == PROCESS_STATE_UNUSED)
        return;

    kfree((void *)process->kernel_stack);
    kfree((void *)process->user_stack);

    if (process->page_directory != NULL)
    {
        // destroy_page_directory(process->page_directory);
        process->page_directory = NULL;
    }

    memset(process->context, 0, sizeof(struct regs));

    memset(process->name, 0, sizeof(process->name));

    process->state = PROCESS_STATE_UNUSED;

    __LOG(__LOG_HEADER, "PROCESS", "Process destroyed");
}

#undef __LOG_HEADER
#undef __LOG