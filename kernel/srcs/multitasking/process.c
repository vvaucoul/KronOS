/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:13:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/17 10:17:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>

#define USER_CODE_OFFSET 0x600000

process_t process_table[MAX_PROCESS];

uint32_t next_pid = 0;
uint32_t nb_process = 0;

void init_process(void)
{
    for (uint32_t i = 0; i < MAX_PROCESS; ++i)
    {
        process_table[i].state = PROCESS_STATE_UNUSED;
    }

    next_pid = 0;
    nb_process = 0;

    __LOG(__LOG_HEADER, "PROCESS", "Initialized");
}

static void __init_process_stack(uint32_t *stack, void (*entry_point)(void))
{
    *(--stack) = (uint32_t)entry_point; // EIP
    *(--stack) = 0x202;                 // EFLAGS
    *(--stack) = 0x0;                   // EAX
    *(--stack) = 0x0;                   // ECX
    *(--stack) = 0x0;                   // EDX
    *(--stack) = 0x0;                   // EBX
    *(--stack) = (uint32_t)&stack;      // ESP
    *(--stack) = 0x0;                   // EBP
    *(--stack) = 0x0;                   // ESI
    *(--stack) = 0x0;                   // EDI
}

static process_t *__alloc_new_process(uint32_t process_size)
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
    else
        __LOG(__LOG_HEADER, "PROCESS", "Found empty slot in process table");

    memset(process, 0, sizeof(process_t));
    process->state = PROCESS_STATE_INITING;

    __LOG(__LOG_HEADER, "PROCESS", "Allocating process stack addr");

    process->stack = (uint32_t)kmalloc(PROCESS_STACK);

    assert(process->stack != 0);

    process->page_directory = kernel_directory;

    process->page = create_user_page(process->stack + USER_CODE_OFFSET, process->stack + USER_CODE_OFFSET + process_size, kernel_directory);

    if (process->page == NULL)
    {
        kfree((void *)process->stack);

        process->state = PROCESS_STATE_UNUSED;

        printk("Failed to allocate process page directory\n");
        return (NULL);
    }

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
    nb_process++;

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
 * @param stack
 * @param entry_point
 * @param level
 * @param size
 * @return
 */
process_t *create_processus(const char *name, struct regs *cpu_state, void *kernel_stack, void (*entry_point)(void), process_level_t level, uint32_t size)
{
    process_t *process = NULL;

    __LOG(__LOG_HEADER, "PROCESS", "Creating init process");

    /*
    Allocate a new process
      - Set the stack pointer
      - Set Page Directory & Page
      - Set the current state
    */
    process = __alloc_new_process(size);
    
    assert(process != NULL);
    assert(process->stack);
    
    __init_process_stack((uint32_t *)(process->stack + PROCESS_STACK), entry_point);

    process_table[process->pid].stack = process->stack + PROCESS_STACK;


    __LOG(__LOG_HEADER, "PROCESS", "Init process created");

    memcpy(process->name, name, strlen(name));
    process->level = level;
    process->priority = 0;

    uint32_t __context = process->stack + PROCESS_STACK - sizeof(struct regs);

    process->context = (struct regs *)__context;

    memset(process->context, 0, sizeof(struct regs));

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

    process->entry_point = entry_point;

    process->state = PROCESS_STATE_READY;

    __LOG(__LOG_HEADER, "PROCESS", "Init process ready");

    return (process);
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

    // kfree(&process->stack);

    if (process->page_directory != NULL)
    {
        // destroy_page_directory(process->page_directory);
        process->page_directory = NULL;
    }

    memset(process->context, 0, sizeof(struct regs));
    memset(process->name, 0, sizeof(process->name));

    process->state = PROCESS_STATE_UNUSED;

    nb_process--;

    bzero(&process_table[process->pid], sizeof(process_t));

    __LOG(__LOG_HEADER, "PROCESS", "Process destroyed");
}

#undef __LOG_HEADER
#undef __LOG