/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:13:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/01 16:37:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>
#include <memory/memory.h>
#include <multitasking/process.h>

#include <system/tss.h>

#include <asm/asm.h>

volatile task_t *current_task;
volatile task_t *ready_queue;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

extern uint32_t read_eip(void);

extern uint32_t initial_esp;

static int32_t next_pid = 1;

static void move_stack(void *new_stack_start, uint32_t size) {
    uint32_t i, pd_addr, old_stack_pointer, old_base_pointer, new_stack_pointer, new_base_pointer, offset, tmp, *tmp2;

    /* Allocate some space for the new stack */
    for (i = (uint32_t)new_stack_start; i >= ((uint32_t)new_stack_start - size); i -= 0x1000) {
        /* General-purpose stack is in user-mode */
        const page_t *page = get_page(i, current_directory);
        if (!page) {
            page = create_page(i, current_directory);
        }
        alloc_frame((page_t *)page, 0, 1);
    }

    /* Flush the TLB by reading and writing the page directory address again */
    __asm__ __volatile__("mov %%cr3, %0"
                         : "=r"(pd_addr));
    __asm__ __volatile__("mov %0, %%cr3"
                         :
                         : "r"(pd_addr));

    /* Old ESP and EBP, read from registers */
    __asm__ __volatile__("mov %%esp, %0"
                         : "=r"(old_stack_pointer));
    __asm__ __volatile__("mov %%ebp, %0"
                         : "=r"(old_base_pointer));

    /* Offset to add to old stack addresses to get a new stack address */
    offset = (uint32_t)new_stack_start - initial_esp;

    /* New ESP and EBP */
    new_stack_pointer = old_stack_pointer + offset;
    new_base_pointer = old_base_pointer + offset;

    /* Copy the stack */
    memcpy((void *)new_stack_pointer, (void *)old_stack_pointer, initial_esp - old_stack_pointer);

    /* Backtrace through the original stack, copying new values into the new stack */
    for (i = (uint32_t)new_stack_start; i > (uint32_t)new_stack_start - size; i -= 4) {
        tmp = *(uint32_t *)i;
        if ((old_stack_pointer < tmp) && (tmp < initial_esp)) {
            tmp = tmp + offset;
            tmp2 = (uint32_t *)i;
            *tmp2 = tmp;
        }
    }

    /* Change stacks */
    __asm__ __volatile__("mov %0, %%esp"
                         :
                         : "r"(new_stack_pointer));
    __asm__ __volatile__("mov %0, %%ebp"
                         :
                         : "r"(new_base_pointer));
}

void init_tasking(void) {
    ASM_CLI();

    printk("Initialising tasking...\n");

    move_stack((void *)0xE0000000, 0x2000);

    /* Initialise the first task (kernel task) */
    current_task = ready_queue = (task_t *)kmalloc(sizeof(task_t));

    current_task->pid = next_pid++;
    current_task->ppid = 0;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->next = 0;
    current_task->kernel_stack = (uint32_t)kmalloc_a(KERNEL_STACK_SIZE);

    printk("Initialised tasking\n");

    ASM_STI();
}

int32_t task_fork(void) {
    uint32_t eip, esp, ebp;
    task_t *parent_task, *new_task, *tmp_task;
    page_directory_t *directory;

    /* We are modifying kernel structures, and so cannot */
    ASM_CLI();

    /* Take a pointer to this process' task struct for later reference */
    parent_task = (task_t *)current_task;

    /* Clone the address space */
    directory = clone_page_directory(current_directory);

    /* Create a new process */
    new_task = (task_t *)kmalloc(sizeof(task_t));

    new_task->pid = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    current_task->kernel_stack = (uint32_t)kmalloc_a(KERNEL_STACK_SIZE);
    new_task->next = 0;

    /* Add it to the end of the ready queue */
    tmp_task = (task_t *)ready_queue;
    while (tmp_task->next)
        tmp_task = tmp_task->next;
    tmp_task->next = new_task;

    /* This will be the entry point for the new process */
    eip = read_eip();

    /* We could be the parent or the child here - check */
    if (current_task == parent_task) {
        /* We are the parent, so set up the esp/ebp/eip for our child */
        __asm__ __volatile__("mov %%esp, %0"
                             : "=r"(esp));
        __asm__ __volatile__("mov %%ebp, %0"
                             : "=r"(ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;
        new_task->ppid = parent_task->pid;
        ASM_STI();

        return new_task->pid;
    } else {
        /* We are the child */
        return 0;
    }
}

int32_t getpid(void) {
    return current_task->pid;
}
int32_t getppid(void) {
    return current_task->ppid;
}

task_t *get_current_task(void) {
    return (task_t *)current_task;
}
task_t *get_task(int32_t pid) {
    task_t *tmp_task = (task_t *)ready_queue;

    /* Find the process in the proc. list */
    while (tmp_task->pid != pid) {
        tmp_task = tmp_task->next;
        if (!tmp_task)
            break;
    }
    return tmp_task;
}

int32_t init_task(void func(void)) {
    int32_t ret = task_fork();
    int32_t pid = getpid();

    /* If we are the child */
    if (!ret) {
        /* Execute the requested function */
        func();
        /* Kill the current (child) process. On failure, freeze it */
        printk("Child process returned from init_task\n");
        if (kill_task(pid) != 0) {
            printk("Child process failed to die, freezing\n");
            for (;;)
                ;
        }
        printk("Child process failed to die\n");
    }
    return ret;
}

int32_t kill_task(int32_t pid) {
    task_t *tmp_task;
    task_t *par_task;

    printk("Killing task: %d\n", pid);

    if (!pid)
        return 0;

    tmp_task = get_task(pid);
    if (!tmp_task)
        return 0;
        
    printk("Found task: %d\n", tmp_task->pid);

    printk("Parent task: %d\n", tmp_task->ppid);

    /* Can we delete it? */
    if (tmp_task->ppid) {
        par_task = get_task(tmp_task->ppid);
        /* If its stack is reachable, delete it */
        if (tmp_task->kernel_stack)
            kfree((void *)tmp_task->kernel_stack);
        par_task->next = tmp_task->next;
        kfree((void *)tmp_task);

        ksleep(1);

        printk("Killed task: %d\n", pid);
        return pid;
    } else {
        return 0;
    }
}

void switch_to_user_mode(void) {
    tss_set_stack_pointer(current_task->kernel_stack + KERNEL_STACK_SIZE);

    /* Set up a stack structure for switching to user mode */
    __asm__ __volatile__("cli; \
	mov $0x23, %ax; \
	mov %ax, %ds; \
	mov %ax, %es; \
	mov %ax, %fs; \
	mov %ax, %gs; \
	mov %esp, %eax; \
	pushl $0x23; \
	pushl %esp; \
	pushf; \
	pushl $0x1B; \
	push $1f; \
	sti; \
	iret; \
	1: \
	");
}