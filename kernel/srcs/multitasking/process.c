/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:13:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/02 16:38:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>
#include <memory/memory.h>

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

#include <system/tss.h>

#include <asm/asm.h>

#include <kernel.h>

task_t *current_task = NULL;
// volatile task_t *scheduler_tasks[MAX_TASKS] = {NULL};
uint32_t num_tasks = 0;

task_t *ready_queue = NULL;

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
    // printk("\n\n--- INIT TASKING ---\n\n");

    ASM_CLI();

    // printk("\t- Move stack\n");

    move_stack((void *)0xE0000000, 0x2000);

    /* Initialise the first task (kernel task) */
    current_task = ready_queue = (task_t *)kmalloc(sizeof(task_t));

    if (!(current_task))
        __THROW_NO_RETURN("init_tasking : kmalloc failed");

    current_task->pid = next_pid++;
    current_task->ppid = 0;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->next = current_task->prev = NULL;
    current_task->kernel_stack = (uint32_t)kmalloc_a(KERNEL_STACK_SIZE);
    current_task->exit_code = 0;
    current_task->state = TASK_RUNNING;
    current_task->owner = 0;

    if (!(current_task->kernel_stack))
        __THROW_NO_RETURN("init_tasking : kmalloc_a failed");

    // printk("\t- Current task : %d\n", current_task->pid);
    
    ASM_STI();
}

int32_t task_fork(void) {
    // printk("\t- Fork\n");

    uint32_t eip, esp, ebp;
    task_t *parent_task, *new_task, *tmp_task;
    page_directory_t *directory;

    /* We are modifying kernel structures, and so cannot preempt */
    ASM_CLI();

    /* Take a pointer to this process' task struct for later reference */
    parent_task = (task_t *)current_task;
    // printk("\t- Parent task : %d\n", parent_task->pid);

    /* Clone the address space */
    if (!(directory = clone_page_directory(current_directory)))
        __THROW("task_fork : clone_page_directory failed", 1);

    /* Create a new process */
    if (!(new_task = (task_t *)kmalloc(sizeof(task_t))))
        __THROW("task_fork : kmalloc failed", 1);

    new_task->pid = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    current_task->kernel_stack = (uint32_t)kmalloc_a(KERNEL_STACK_SIZE);
    new_task->next = NULL;
    new_task->prev = NULL; // Set prev task when added to ready queue
    new_task->exit_code = 0;
    new_task->state = TASK_RUNNING;
    new_task->owner = 0;

    if (!(current_task->kernel_stack))
        __THROW("task_fork : kmalloc failed", 1);

    /* Add it to the end of the ready queue */
    tmp_task = (task_t *)ready_queue;
    while (tmp_task->next)
        tmp_task = tmp_task->next;
    tmp_task->next = new_task;
    new_task->prev = tmp_task;

    // printk("\t- Prev Task [%d] -> Task [%d] -> Next Task [%d]\n", tmp_task->pid, new_task->pid, new_task->next == NULL ? -1 : new_task->next->pid);

    /* This will be the entry point for the new process */
    eip = read_eip();

    // printk("\t- EIP : %x\n", eip);
    // printk("\t- current_task == parent_task : %d\n", current_task == parent_task);

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

        return (new_task->pid);
    } else {
        /* We are the child */
        ASM_STI();
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

    if (!tmp_task) {
        return NULL;
    }

    /* Find the process in the proc. list */
    while (tmp_task->pid != pid) {
        tmp_task = tmp_task->next;
        if (!tmp_task)
            break;
    }
    return tmp_task;
}

int32_t init_task(void func(void)) {
    // printk("\n\n--- INIT TASK ---\n\n");

    int32_t ret = task_fork();
    int32_t pid = getpid();

    /* If we are the child */
    if (!ret) {
        /* Execute the requested function */
        func();
        /* Kill the current (child) process. On failure, freeze it */
        // printk("Child process returned from init_task\n");
        if (kill_task(pid) != 0) {
            // printk("Child process failed to die, freezing\n");
            for (;;)
                ;
        }
        // printk("Child process failed to die\n");
    }
    return ret;
}

int32_t kill_task(int32_t pid) {
    task_t *tmp_task;
    task_t *par_task;

    // printk("\n\n--- KILL TASK ---\n\n");

    // printk("Killing task: %d\n", pid);

    if (!pid)
        return 0;

    tmp_task = get_task(pid);
    if (!tmp_task)
    {
        __THROW("kill_task : task not found for pid %d", -1, pid);
    }

    // printk("Found task: %d\n", tmp_task->pid);
    // printk("Parent task: %d\n", tmp_task->ppid);

    /* Can we delete it? */
    if (tmp_task->ppid != 0) {
        par_task = get_task(tmp_task->ppid);
        // printk("Found parent task: %d\n", par_task->pid);
        /* If its stack is reachable, delete it */
        if (tmp_task->kernel_stack) {
            kfree((void *)tmp_task->kernel_stack);
            // printk("Freed kernel stack\n");
        }
        par_task->next = tmp_task->next;
        kfree((void *)tmp_task);

        // printk("Waiting for task %u to die\n", pid);
        ksleep(1);
        // printk("Killed task: %d\n", pid);
        return (pid);
    } else {
        return (0);
    }
}

void exit_task(uint32_t retval) {
    kill_task(current_task->pid);
    switch_to_user_mode();
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