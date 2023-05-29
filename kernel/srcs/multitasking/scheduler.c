/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 15:28:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/scheduler.h>

bool scheduler_initialized = false;

int32_t get_current_pid(void)
{
    if (!scheduler_initialized)
        __PANIC("Scheduler not initialized");
    if (!current_process)
        return -1;
    return current_process->pid;
}

void init_scheduler(void)
{
    memset(scheduler_queue, 0, MAX_PROCESSES * sizeof(pcb_t));

    for (uint32_t i = 0; i < MAX_PROCESSES; i++)
    {
        scheduler_queue[i].pid = i;
        scheduler_queue[i].state = PROCESS_TERMINATED;
        scheduler_queue[i].parent = NULL;
        scheduler_queue[i].children = NULL;
        scheduler_queue[i].stack_pointer = NULL;
        scheduler_queue[i].heap_pointer = NULL;
        scheduler_queue[i].owner_id = -1;
    }

    // Initialize current process
    current_process = &scheduler_queue[0];
    current_process->state = PROCESS_READY;

    scheduler_initialized = true;
}

void enqueue_process(uint32_t pid)
{
    if ((queue_end + 1) % MAX_PROCESSES == queue_start)
    {
        // There is no room in the queue
        __PANIC("Scheduler queue is full");
    }

    // There is room in the queue
    scheduler_queue[queue_end].pid = pid;
    queue_end = (queue_end + 1) % MAX_PROCESSES;
    printk("Process %d added to scheduler queue\n", pid);
}

void dequeue_process(uint32_t pid)
{
    // This function removes the process from anywhere in the queue
    for (uint32_t i = queue_start; i != queue_end; i = (i + 1) % MAX_PROCESSES)
    {
        if (scheduler_queue[i].pid == pid)
        {
            // Shift all later processes one step towards the start of the queue
            uint32_t j = i;
            while (j != queue_end)
            {
                uint32_t next_j = (j + 1) % MAX_PROCESSES;
                scheduler_queue[j] = scheduler_queue[next_j];
                j = next_j;
            }

            // Update the end of the queue
            queue_end = (queue_end + MAX_PROCESSES - 1) % MAX_PROCESSES;

            // Clear the structure of the last process in the queue
            // so that it doesn't remain in the queue twice
            scheduler_queue[queue_end] = (pcb_t){0};

            printk("Process %d removed from scheduler queue\n", pid);

            return;
        }
    }
    __PANIC("Process not found in scheduler queue");
}

// Add this function somewhere in your code
void end_process(void)
{
    printk("Process %d ended\n", get_current_pid());
    kpause();

    // Get the current PID somehow. This will depend on your OS design.
    uint32_t current_pid = get_current_pid();

    // Mark process as terminated
    pcb_t *process = &scheduler_queue[current_pid];
    process->state = PROCESS_TERMINATED;

    // Remove process from scheduler queue
    dequeue_process(current_pid);

    // Save current process context before switching to the next process
    __asm__ volatile("movl %%esp, %0"
                     : "=m"(process->stack_pointer));

    // Call the scheduler to switch to the next process
    scheduler();
}

int create_process(void (*entry_point)(void))
{
    // Find an empty slot in the process table
    uint32_t i;
    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (scheduler_queue[i].state == PROCESS_TERMINATED)
            break;
    }

    if (i == MAX_PROCESSES)
    {
        __ERROR(__E_PROCESS_CREATION);
        return ERROR_NO_MORE_PROCESSES;
    }

    // Initialize process control block
    pcb_t *process = &scheduler_queue[i];

    process->pid = i;
    process->state = PROCESS_READY;
    process->parent = current_process;
    process->children = NULL;

    process->stack_pointer = kmalloc(STACK_SIZE);
    if (process->stack_pointer == NULL)
    {
        __ERROR("Unable to allocate stack for the process");
        return ERROR_STACK_ALLOC_FAILED;
    }
    memset(process->stack_pointer, 0, STACK_SIZE);

    process->heap_pointer = kmalloc(sizeof(int) * HEAP_SIZE);

    if (process->heap_pointer == NULL)
    {
        kfree(process->stack_pointer); // Free stack since we couldn't allocate heap
        __ERROR("Unable to allocate heap for the process");
        return ERROR_HEAP_ALLOC_FAILED;
    }
    memset(process->heap_pointer, 0, HEAP_SIZE * sizeof(int));

    process->owner_id = 0;

    // Initialize process stack with entry point and dummy return address
    void **stack_top = (void **)((char *)process->stack_pointer + STACK_SIZE);
    *(--stack_top) = (void *)entry_point;
    *(--stack_top) = NULL;

    // Save stack pointer in process control block
    process->stack_pointer = stack_top;

    assert(process->state == PROCESS_READY);
    assert(process->parent == current_process);
    assert(process->children == NULL);
    assert(process->stack_pointer != NULL);
    assert(process->heap_pointer != NULL);
    assert(process->owner_id == 0);

    printk("Process %d created\n", process->pid);
    enqueue_process(i);

    return (0);
}

void delete_process(uint32_t pid)
{
    // k process stack and mark process as terminated
    pcb_t *process = &scheduler_queue[pid];
    kfree(process->stack_pointer);
    kfree(process->heap_pointer);

    // ATOMIC INSTRUCTION: Compilet Barrier -> No reorder
    __sync_synchronize();

    process->state = PROCESS_TERMINATED;

    // Remove process from scheduler queue
    dequeue_process(pid);
}

void block_process(uint32_t pid)
{
    // Mark process as blocked
    pcb_t *process = &scheduler_queue[pid];
    process->state = PROCESS_BLOCKED;

    // Remove process from scheduler queue
    dequeue_process(pid);
}

void unblock_process(uint32_t pid)
{
    // Mark process as ready
    pcb_t *process = &scheduler_queue[pid];
    process->state = PROCESS_READY;

    // Add process to scheduler queue
    enqueue_process(pid);
}

void switch_context(pcb_t **old_process, pcb_t *new_process)
{
    __asm__ volatile(
        // Sauvegarder l'état du processus actuel
        "pushl %%ebp\n"
        "pushl %%ebx\n"
        "pushl %%esi\n"
        "pushl %%edi\n"
        "movl %%esp, %0\n"
        : "=m"((*old_process)->stack_pointer)
        :
        : "memory");

    // Restaurer l'état du nouveau processus
    __asm__ volatile(
        "movl %0, %%esp\n"
        "popl %%edi\n"
        "popl %%esi\n"
        "popl %%ebx\n"
        "popl %%ebp\n"
        :
        : "r"(new_process->stack_pointer)
        : "memory");
}

uint32_t current_process_index = 0;
pcb_t *next_process = NULL;

void scheduler(void)
{
    // pcb_t *next_process = NULL;
    // for (uint32_t i = 0; i < MAX_PROCESSES; i++)
    // {
    //     if (scheduler_queue[i].state == PROCESS_READY)
    //     {
    //         // if (next_process == NULL || scheduler_queue[i].pid < next_process->pid)
    //         if (next_process == NULL)
    //         {
    //             next_process = &scheduler_queue[i];
    //         }
    //     }
    // }

    // Find next process in scheduler queue
    if (scheduler_queue[current_process_index].state != PROCESS_READY)
    {
        for (uint32_t i = 0; i < MAX_PROCESSES; i++)
        {
            if (scheduler_queue[i].state == PROCESS_READY)
            {
                next_process = &scheduler_queue[i];
                current_process_index = (i + 1) % MAX_PROCESSES;
                break;
            }
        }
    }
    else
    {
        next_process = &scheduler_queue[current_process_index];
        current_process_index = (current_process_index + 1) % MAX_PROCESSES;
    }

    if (next_process == NULL)
    {
        __ERROR(__E_NO_PROCESS);
        return;
    }

    printk("Next process: %s\n", next_process != NULL ? "VALID" : "NULL");
    printk("Current process: %s\n", current_process != NULL ? "VALID" : "NULL");

    // Switch to next process if it's different from the current process
    if (next_process != current_process)
    {
        // Switch to next process
        pcb_t *old_process = current_process;
        current_process = next_process;
        current_process->state = PROCESS_RUNNING;

        // Check heap and stack for process
        {
            uint32_t *heap = (uint32_t *)current_process->heap_pointer;
            uint32_t *stack = (uint32_t *)current_process->stack_pointer;

            if (*heap == 0x0)
            {
                __ERROR(__E_HEAP_CORRUPTED);
            }

            if (*stack == 0x0)
            {
                __ERROR(__E_STACK_CORRUPTED);
            }

            uint32_t *heap_op = (uint32_t *)old_process->heap_pointer;
            uint32_t *stack_op = (uint32_t *)old_process->stack_pointer;

            if (*heap_op == 0x0)
            {
                __ERROR(__E_HEAP_CORRUPTED);
            }

            if (*stack_op == 0x0)
            {
                __ERROR(__E_STACK_CORRUPTED);
            }
        }

        printk("Switching from process %d to process %d\n", old_process->pid, current_process->pid);

        switch_context(&old_process, current_process);

        printk("Switched from process %d to process %d\n", old_process->pid, current_process->pid);
    }
    else
    {
        printk("No switch necessary. Current: %d, Next: %d\n", current_process ? current_process->pid : -1, next_process ? next_process->pid : -1);
    }
}