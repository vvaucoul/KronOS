/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:13:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/25 14:05:38 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/frames.h>
#include <memory/kheap.h>
#include <memory/memory.h>

#include <multitasking/process.h>
#include <multitasking/scheduler.h>

#include <system/tss.h>

#include <system/time.h>

#include <asm/asm.h>

#include <kernel.h>

task_t *current_task = NULL;
uint32_t num_tasks = 0;

task_t *ready_queue = NULL;
task_t *wait_queue = NULL;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

extern uint32_t read_eip(void);

extern uint32_t initial_esp;

// static int32_t next_pid = 1;

static void move_stack(void *new_stack_start, uint32_t size) {
    uint32_t i, pd_addr, old_stack_pointer, old_base_pointer, new_stack_pointer, new_base_pointer, offset, tmp, *tmp2;

    /* Allocate some space for the new stack */
    for (i = (uint32_t)new_stack_start; i >= ((uint32_t)new_stack_start - size); i -= PAGE_SIZE) {
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

static void __process_sectors(task_t *process) {
    /* Initialise BSS / DATA segment */
    process->sectors.bss_size = BSS_SIZE;
    if (!(process->sectors.bss_segment = kmalloc(process->sectors.bss_size)))
        __THROW_NO_RETURN("init_tasking : kmalloc failed");
    memset(process->sectors.bss_segment, 0, process->sectors.bss_size);

    process->sectors.data_size = DATA_SIZE;
    if (!(process->sectors.data_segment = kmalloc(process->sectors.data_size)))
        __THROW_NO_RETURN("init_tasking : kmalloc failed");

    /* Copy initial data */
    memcpy(process->sectors.data_segment, process->sectors.data_segment, process->sectors.data_size);
}

void init_tasking(void) {
    // printk("\n\n--- INIT TASKING ---\n\n");

    ASM_CLI();

    // printk("\t- Move stack\n");

    move_stack((void *)0xDEADBEEF, KERNEL_STACK_SIZE);

    /* Initialise the first task (kernel task) */
    current_task = ready_queue = (task_t *)kmalloc_a(sizeof(task_t));

    if (!(current_task))
        __THROW_NO_RETURN("init_tasking : kmalloc failed");

    memset(current_task, 0, sizeof(task_t));

    // current_task->pid = next_pid++;
    current_task->pid = find_first_free_pid();
    current_task->ppid = 0;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->next = current_task->prev = NULL;
    if (!(current_task->kernel_stack = (uint32_t)kmalloc_a(KERNEL_STACK_SIZE)))
        __THROW_NO_RETURN("init_tasking : kmalloc_a failed");
    current_task->state = TASK_RUNNING;
    current_task->owner = current_task->effective_owner = 0;
    current_task->task_id = (struct task_id_t){0, 0, 0, 0};
    current_task->cpu_load = (process_cpu_load_t){0, 0, 0};
    current_task->signal_queue = NULL;
    current_task->or_priority = current_task->priority = TASK_PRIORITY_LOW;
    current_task->zombie_hungry = 0;

    __process_sectors(current_task);

    wait_queue = NULL;

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

    /* Clone the address space */
    if (!(directory = clone_page_directory(current_directory)))
        __THROW("task_fork : clone_page_directory failed", 1);

    /* Create a new process */
    if (!(new_task = (task_t *)kmalloc_a(sizeof(task_t))))
        __THROW("task_fork : kmalloc failed", 1);

    memset(new_task, 0, sizeof(task_t));

    // new_task->pid = next_pid++;
    new_task->pid = find_first_free_pid();
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    new_task->kernel_stack = (uint32_t)kmalloc_a(KERNEL_STACK_SIZE);
    new_task->next = NULL;
    new_task->prev = NULL; // Set prev task when added to ready queue
    new_task->exit_code = 0;
    new_task->state = TASK_RUNNING;
    new_task->owner = new_task->effective_owner = 0;
    new_task->task_id = (struct task_id_t){0, 0, 0, 0};
    new_task->cpu_load = (process_cpu_load_t){0, 0, 0};
    new_task->signal_queue = NULL;
    new_task->or_priority = new_task->priority = TASK_PRIORITY_MEDIUM;
    new_task->zombie_hungry = 0;

    __process_sectors(new_task);

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
        return (0);
    }
}

__attribute__((pure)) page_directory_t *get_task_directory(void) {
    return current_task->page_directory;
}

void set_task_uid(task_t *task, uint32_t uid) {
    task->task_id.uid = uid;
}

void set_task_gid(task_t *task, uint32_t gid) {
    task->task_id.gid = gid;
}

void set_task_euid(task_t *task, uint32_t euid) {
    task->task_id.euid = euid;
}

void set_task_egid(task_t *task, uint32_t egid) {
    task->task_id.egid = egid;
}

pid_t getpid(void) {
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

    int32_t ret = task_fork();
    int32_t pid = getpid();

    /* If we are the child */
    if (!ret) {
        /* Execute the requested function */
        func();
        /* Set task to running state */
        // get_task(pid)->state = TASK_RUNNING;
        /* Kill the current (child) process. On failure, freeze it */

        if (get_task(pid)->state == TASK_RUNNING) {
            __WARND("Task ended without calling task_exit");
            if (kill_task(pid) != 0) {
                printk("init_Task 1\n");
                get_task(pid)->state = TASK_ZOMBIE;
                printk("init_Task 2\n");
                for (;;)
                    ;
            }
        }
        // Not sure about that, to check
        else {
            // __WARND("Task ended without calling task_exit");
            // printk("init_Task 3\n");
            // get_task(pid)->state = TASK_ZOMBIE;
            // printk("init_Task 4\n");
            for (;;)
                ;
        }
    }
    return ret;
}

/**
 * @brief Wait for a task to exit
 * @param pid
 * @return int32_t
 * @note : Wait for a task to exit and return its exit code
 *        If the task is still running, yield the CPU
 */
int32_t task_wait(int32_t pid) {
    task_t *task = get_task(pid);
    if (!task) {
        __WARN("task_wait : task not found for pid %d", -1, pid);
    }

    while (1) {
        // If the task has exited, return its exit code
        if (task->state == TASK_ZOMBIE) {
            int exit_code = task->exit_code;
            task->state = TASK_STOPPED;
            printk("Task [%d] exited with code %d\n", pid, exit_code);
            return exit_code;
        } else {
            // If the task is still running, yield the CPU
            busy_wait((100 * TIMER_PHASE) / 1000);
            // printk("Task [%d] state : %d\n", pid, task->state);
        }
    }

    return (-1);
}

int32_t kill_task(int32_t pid) {
    task_t *tmp_task;

    if (!pid) {
        __WARN("kill_task : cannot kill kernel task", 0);
    } else if (pid == 1) {
        __WARN("kill_task : cannot kill INIT task", 0);
    }

    tmp_task = get_task(pid);
    if (!tmp_task) {
        __THROW("kill_task : task not found for pid %d", 0, pid);
    }

    if (tmp_task->ppid != 0) {
        /* If its stack is reachable, delete it */
        if (tmp_task->kernel_stack) {
            kfree((void *)tmp_task->kernel_stack);
            tmp_task->kernel_stack = 0x0;
        }

        /* Set all children to Zombies */
        if (tmp_task->next) {
            task_t *tmp = get_task(tmp_task->next->pid);
            while (tmp) {
                printk("[%d] -> Set task orphan [%d]\n", pid, tmp->pid);
                tmp->state = TASK_ORPHAN;

                // Todo: Unix system like : send SIGCHLD to parent to get exit code
                // signal(tmp_task->ppid, SIGCHLD);
                tmp = tmp->next;
            }
        }

        tmp_task->state = TASK_ZOMBIE; // works to waitpid
        // tmp_task->state = TASK_STOPPED; // works to stop while task immediatly
        
        // kmsleep(TASK_FREQUENCY);
        // busy_wait((1 * TIMER_PHASE) / 1000); // Wait 1 second

        // kpause();
        return (pid);

        //  /* Relink the previous and next tasks around the one we're removing */
        // if (tmp_task->prev != NULL) {
        //     tmp_task->prev->next = tmp_task->next;
        // } else {
        //     ready_queue = tmp_task->next;
        // }
        // if (tmp_task->next != NULL) {
        //     tmp_task->next->prev = tmp_task->prev;
        // }
        // printk("2\n");

        // // printk("Prev Task [%d] -> Task [%d] -> Next Task [%d]\n",
        // //        tmp_task->prev ? tmp_task->prev->pid : -1,
        // //        tmp_task->pid,
        // //        tmp_task->next ? tmp_task->next->pid : -1);

        // // Todo: free page directory: Currently crash
        // // destroy_page_directory(tmp_task->page_directory);

        // kfree(tmp_task->sectors.bss_segment);
        // kfree(tmp_task->sectors.data_segment);

        // kfree((void *)tmp_task);
        // printk("3\n");

        // // busy_wait((100 * TIMER_PHASE) / 1000); // Wait 1 second
        // printk("4\n");
        // kmsleep(TASK_FREQUENCY);
        // return (pid);
    } else {
        __WARN("kill_task : cannot kill kernel task %d", 0, pid);
    }
}

/**
 * @brief Free a task
 * @param task
 * @return int32_t
 * @note : Called only by scheduler to free a task
 *         First, terminate task, then wait for scheduler to find it and free it
 */
int32_t free_task(task_t *task) {
    if (!task) {
        __WARN("free_task : task is NULL", -1);
    } else if (task->pid == 0 || task->pid == 1) {
        __WARN("free_task : cannot free kernel task", -1);
    } else if (task->state == TASK_RUNNING) {
        __WARN("free_task : cannot free running task", -1);
        return kill_task(task->pid);
    } else {

        pid_t task_pid = task->pid;

        task->state = TASK_STOPPED;

        /* Relink the previous and next tasks around the one we're removing */
        if (task->prev != NULL) {
            task->prev->next = task->next;
        } else {
            ready_queue = task->next;
        }
        if (task->next != NULL) {
            task->next->prev = task->prev;
        }

        // Todo: free page directory: Currently crash
        // destroy_page_directory(task->page_directory);

        kfree(task->sectors.bss_segment);
        kfree(task->sectors.data_segment);

        kfree((void *)task);
        
        // busy_wait((100 * TIMER_PHASE) / 1000); // Wait 1 second
        // kmsleep(TASK_FREQUENCY);
        return (task_pid);
    }
    return (-1);
}

int32_t kill_all_tasks(void) {
    task_t *tmp_task = ready_queue;
    while (tmp_task) {
        if (tmp_task->pid != 0 && tmp_task->pid != 1) {
            kill_task(tmp_task->pid);
        }
        tmp_task = tmp_task->next;
    }
    return (0);
}

void lock_task(task_t *task) {
    ASM_CLI();
    task->state = TASK_WAITING;
    task->next = wait_queue;
    wait_queue = task;
    ASM_STI();
}

void unlock_task(task_t *task) {
    ASM_CLI();
    if (wait_queue) {
        task_t *tmp = wait_queue;
        if (tmp == task) {
            wait_queue = tmp->next;
        } else {
            while (tmp->next && tmp->next != task)
                tmp = tmp->next;
            if (tmp->next) {
                tmp->next = task->next;
            }
        }
        task->next = NULL;
        task->state = TASK_RUNNING;
    }
    ASM_STI();
}

void task_exit(int32_t retval) {
    get_current_task()->exit_code = retval;
    get_current_task()->state = TASK_ZOMBIE;

    /* Kill task before task reach the scheduler */
    kill_task(get_current_task()->pid);
}

void switch_to_user_mode(void) {
    tss_set_stack_pointer(current_task->kernel_stack + KERNEL_STACK_SIZE);

    /* Set up a stack structure for switching to user mode */
    // __asm__ __volatile__("cli; \
	// mov $0x23, %ax; \
	// mov %ax, %ds; \
	// mov %ax, %es; \
	// mov %ax, %fs; \
	// mov %ax, %gs; \
	// mov %esp, %eax; \
	// pushl $0x23; \
	// pushl %esp; \
	// pushf; \
	// pushl $0x1B; \
	// push $1f; \
	// sti; \
	// iret; \
	// 1: \
	// ");

    // unsigned int eflags, cs, ds, es, fs, gs;

    // __asm__ volatile("pushf; pop %%eax" : "=a"(eflags));
    // __asm__ volatile("mov %%cs, %%eax" : "=a"(cs));
    // __asm__ volatile("mov %%ds, %%eax" : "=a"(ds));
    // __asm__ volatile("mov %%es, %%eax" : "=a"(es));
    // __asm__ volatile("mov %%fs, %%eax" : "=a"(fs));
    // __asm__ volatile("mov %%gs, %%eax" : "=a"(gs));

    // printk("EFLAGS: 0x%x\n", eflags);
    // printk("CS: 0x%x\n", cs);
    // printk("DS: 0x%x\n", ds);
    // printk("ES: 0x%x\n", es);
    // printk("FS: 0x%x\n", fs);
    // printk("GS: 0x%x\n", gs);

    // switch_user_mode();
    kpause();

    __asm__ __volatile__("cli; \
	mov $0x2B, %ax; \
	mov %ax, %ds; \
	mov %ax, %es; \
	mov %ax, %fs; \
	mov %ax, %gs; \
	mov %esp, %eax; \
	pushl $0x2B; \
	pushl %esp; \
	pushf; \
	pushl $0x23; \
	push $1f; \
	sti; \
	iret; \
	1: \
	");
}

pid_t find_first_free_pid(void) {
    pid_t pid = 1;
    task_t *task = NULL;

    // Safeguard against overflow
    while (pid < PID_MAX) {
        task = ready_queue;
        while (task) {
            if (task->pid == pid) {
                pid++;
                break;
            }
            task = task->next;
        }
        // If loop finished without a break, PID is free
        if (!task) {
            return (pid);
        }
    }

    // Log or handle the error - no free PIDs
    __THROW("find_first_free_pid : no free PIDs", -1);
}

void task_set_priority(pid_t pid, task_priority_t priority) {
    task_t *task = get_task(pid);
    if (!task) {
        printk("Invalid PID: %d\n", pid);
        return;
    }

    task->priority = priority;
}

bool is_pid_valid(int pid) {
    return get_task(pid) != NULL;
}

task_t *get_wait_queue(void) {
    return wait_queue;
}

uint32_t get_task_count(void) {
    task_t *task = ready_queue;

    uint32_t count = 0;
    while (task) {
        if (task->pid != 0 && task->pid != 1) {
            count++;
        }
        task = task->next;
    }
    return count;
}

double get_cpu_load(task_t *task) {
    return task->cpu_load.total_load_time;
    uint64_t sys_time = get_system_time();
    uint64_t elapsed_time = sys_time - task->cpu_load.start_time;

    if (elapsed_time == 0) {
        return 0.0;
    }
    return ((double)(task->cpu_load.load_time / elapsed_time) * 100.0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                      UTILS                                     ||
// ! ||--------------------------------------------------------------------------------||

void print_task_info(task_t *task) {
    printk("Task PID: "_GREEN
           "[%d]"_END
           ", Parent PID: "_GREEN
           "[%d]"_END
           ", Owner: "_GREEN
           "[%d]"_END
           ", State: "_GREEN
           "[%d]"_END
           "\n",
           task->pid, task->ppid, task->owner, task->state);
}

void print_all_tasks() {
    task_t *task = ready_queue;
    while (task) {
        print_task_info(task);
        task = task->next;
    }
}

void print_parent_and_children(int pid) {
    task_t *parent_task = get_task(pid);
    if (!parent_task) {
        printk("Invalid PID: %d\n", pid);
        return;
    }

    print_task_info(parent_task);

    printk("Children:\n");
    task_t *task = ready_queue;
    while (task) {
        if (task->ppid == pid) {
            print_task_info(task);
            if (task->next) {
                print_parent_and_children(task->next->pid);
            } else {
                printk("No more children\n");
                return;
            }
        }
        task = task->next;
    }
}