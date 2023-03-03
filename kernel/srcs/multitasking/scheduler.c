/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/17 11:46:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <multitasking/scheduler.h>
#include <system/isr.h>

static cpu_state_t cpu_state;

static bool __check_process(process_t *process)
{
    /* PROCESS INFO */
    if (process == NULL)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process is NULL") | false);
    if (strlen(process->name) == 0)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Name is Empty") | false);
    else if (process->pid > MAX_PROCESS)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process PID is too high") | false);
    else if (process->priority > MAX_PRIORITY)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Priority is too high") | false);

    /* PROCESS MEMORY */
    if (process->context != NULL)
    {
        if (process->context->ebp == 0)
            return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Context EBP is NULL") | false);
        else if (process->context->esp == 0)
            return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Context ESP is NULL") | false);
        else if (process->context->eip == 0)
            return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Context EIP is NULL") | false);
        else
            return true;
    }
    else
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Context is NULL") | false);

    if (process->stack == 0)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Stack is NULL") | false);
    else if (process->page == NULL)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Page is NULL") | false);
    else if (process->page_directory == NULL)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Page Directory is NULL") | false);

    /* PROCESS SIGNALS */

    if (process->sig_queue == NULL)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Signal Queue is NULL") | false);

    /* PROCESS LEVEL */

    if (process->level > 2)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Level is too high") | false);

    /* PROCESS ENTRYPOINT */

    if (process->entry_point == NULL)
        return (__LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process Entry Point is NULL") | false);

    return (true);
}

void init_cpu_state(void)
{
    cpu_state.scheduler = NULL;
    cpu_state.current_process = NULL;

    __LOG(__LOG_HEADER, "SCHEDULER", "Initialized");
}

void init_scheduler(void)
{
    init_cpu_state();
    init_process();
    // ksleep(1);

    /* Scheduler Interrupt for Process Switching */
    __LOG(__LOG_HEADER, "SCHEDULER", "Initializing Interrupt");
    // idt_set_gate(0x20, (unsigned)scheduler_handler, IDT_SELECTOR, IDT_FLAG_GATE);
    // __asm__("int $0x20");
    __LOG(__LOG_HEADER, "SCHEDULER", "Interrupt Initialized");
}

/**
 * @brief Basic Round Robin Scheduler
 *
 */
#include <terminal.h>

static volatile uint32_t current_task = 0;

static void __context_switch(uint32_t *stack)
{
    // Save the current process's context
    __asm__ volatile("pusha");
    __asm__ volatile("mov %%esp, %0"
                     : "=r"(process_table[current_task].stack));

    // Switch to the new process's stack and restore its context
    __asm__ volatile("mov %0, %%esp" ::"r"(stack));
    __asm__ volatile("popa");
    __asm__ volatile("ret");
}

void scheduler(uint32_t ebp, uint32_t esp)
{
    static uint32_t __loop = 0;

    outportb(MASTER_PIC, IRQ_EOI); // Send EOI to Master PIC

    // Display Scheduler Loop
    {
        char buffer[__ITOA_BUFFER_LENGTH__] = {0};

        bzero(buffer, __ITOA_BUFFER_LENGTH__);

        terminal_writestring_location("Scheduler : ", 60, 0);
        uitoa(__loop, buffer);
        terminal_writestring_location(buffer, 60 + 13, 0);
        __loop++;
    }

    if (nb_process == 0)
    {
        __LOG(__LOG_ERROR_HEADER, "SCHEDULER", "No Process to run");
        return;
    }

    uint32_t old_task_idx = current_task;
    uint32_t new_task_idx = current_task++;

    new_task_idx %= MAX_PROCESS;
    current_task = new_task_idx;

    process_t *old_process = &process_table[old_task_idx];
    assert(old_process != NULL);

    printk("Check Process %u\n", old_task_idx);

    if (old_process->state == PROCESS_STATE_RUNNING)
    {
        old_process->context->ebp = ebp;
        old_process->context->esp = esp;
        old_process->state = PROCESS_STATE_READY;

        printk("Old Process : 0x%x - 0x%x\n", old_process->context->ebp, old_process->context->esp);
    }

    process_t *process = &process_table[current_task];
    assert(process != NULL);

    if ((__check_process(process)) == false)
    {
        __LOG(__LOG_ERROR_HEADER, "SCHEDULER", "Process is not valid");
        __PANIC("Process is not valid"); // Trigger Panic

        destroy_processus(process);
        return;
    }

    if (process->state == PROCESS_STATE_READY)
    {

        process->state = PROCESS_STATE_RUNNING;

        printk("New Process : 0x%x - 0x%x\n", process->context->ebp, process->context->esp);

        switch_page_directory(process->page_directory);

        enable_paging((page_directory_t *)&process->page_directory->tablesPhysical);

        assert(process->context != NULL);
        printk("\n\nESP : 0x%x - EBP : 0x%x\n", process->context->esp, process->context->ebp);

        ASM_CLI();

        // Instead, use esp and cr3 (page directory)
        // asm_switch_ucontext(next_task->op_registers.u_esp, next_task->op_registers.cr3);

        __context_switch(&process->stack);

        // context_switch(process->context->esp, process->context->ebp);
        ASM_STI();
        // kpause();

        /*
        ** Todo: Switch user mode
        */

        // assert(process->context != NULL);
        // assert(process->fn != NULL);
        // context_switch_jmp(process->context->esp, process->fn);
    }

    // context_switch(esp, ebp);
    // assert(process->context != NULL);
    // context_switch(process->context->esp, process->context->ebp);
}

// #include <system/panic.h>

// process_t *process_queue;
// process_t *current_process;

// void switch_task(t_regs *regs)
// {
//     memcpy(&current_process->regs, regs, sizeof(t_regs));
//     if (current_process->next != NULL)
//         current_process = current_process->next;
//     else
//         current_process = process_queue;
//     memcpy(regs, &current_process->regs, sizeof(t_regs));
//     switch_page_directory(current_process->page_directory);
// }

// void load_binary(uint8_t *data, uint32_t size)
// {
//     process_t *process = (process_t *)kmalloc(sizeof(process_t));

//     printk("Alloc memory for process 0x%x\n", process);
//     // process->process_page = create_user_page(UCODE_START, UCODE_START + size, current_directory);

//     process->process_page = get_page(UCODE_START, current_directory);
//     if (process->process_page == NULL)
//         process->process_page = create_page(UCODE_START, current_directory);
//     if (process->process_page == NULL)
//         __PANIC("Could not get, create process page");
//     printk("Create / GET user page\n");

//     page_directory_t *tmp = current_directory;

//     printk("Switch page directory\n");

//     switch_page_directory(process->page_directory);
//     kpause();
//     memcpy((void *)UCODE_START, data, size);
//     switch_page_directory(tmp);

//     process->regs.eip = UCODE_START;
//     process->regs.cs = 0x08;
//     process->regs.ds = 0x10;
//     process->regs.ss = 0x10;

//     process->state = PROCESS_STATE_RUNNING;

//     process_t *last_process = process_queue;

//     if (last_process == NULL)
//     {
//         process_queue = process;
//         return;
//     }
//     else
//     {
//         while (last_process->next != NULL)
//             last_process = last_process->next;
//         last_process->next = process;
//     }
// }