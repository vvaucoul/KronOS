/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/26 17:43:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 22:11:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/threads.h>
#include <memory/memory.h>
#include <multitasking/process.h>

#include <macros.h>

thread_t *current_thread = NULL;

static void __thread_add_thread_to_queue(thread_t **list, thread_t *thread) {
    thread_t *tmp = *list;

    if (!tmp) {
        *list = thread;
    } else {
        while (tmp->next) {
            tmp = tmp->next;
        }
        tmp->next = thread;
    }
}

static void __thread_remove_thread_from_queue(thread_t **list, thread_t *thread) {
    thread_t *tmp = *list;
    thread_t *prev = NULL;

    if (!tmp) {
        return;
    } else {
        while (tmp) {
            if (tmp == thread) {
                if (prev) {
                    prev->next = tmp->next;
                } else {
                    *list = tmp->next;
                }
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }
    }
}

static uint32_t __thread_get_thread_tid(thread_t *list) {
    thread_t *tmp = list;

    if (!tmp) {
        return (0);
    } else {
        while (tmp->next) {
            tmp = tmp->next;
        }
        return (tmp->tid + 1);
    }
    return (0);
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    FUNCTIONS                                   ||
// ! ||--------------------------------------------------------------------------------||

void thread_init(void) {
    current_thread = NULL;
}

thread_t *thread_create(void (*func)(void)) {
    thread_t *thread = (thread_t *)kmalloc(sizeof(thread_t));

    if (thread == NULL) {
        __THROW("Failed to create thread", NULL);
    } else {
        if (current_thread == NULL) {
            current_thread = thread;
        }

        memset(thread, 0, sizeof(thread_t));

        thread->eip = 0;
        thread->esp = 0;
        thread->ebp = 0;
        thread->next = NULL;

        thread->func = func;
        thread->wake_up_time = 0;
        thread->state = THREAD_WAITING;

        thread->tid = __thread_get_thread_tid(get_current_task()->threads);

        thread->eip = read_eip();
        thread->esp = (uint32_t)kmalloc(KERNEL_STACK_SIZE);
        thread->ebp = thread->esp + KERNEL_STACK_SIZE;

        printk("Thread "_GREEN
               "[%d]"_END
               " created, ESP: "_GREEN
               "0x%x"_END
               ", EBP: "_GREEN
               "0x%x"_END
               " EIP: "_GREEN
               "0x%x"_END
               "\n",
               thread->tid, thread->esp, thread->ebp, thread->eip);

        __thread_add_thread_to_queue((thread_t **)(&(get_current_task()->threads)), thread);
        // thread_display_threads(get_current_task()->threads);
    }
    return (thread);
}

void thread_destroy(thread_t *thread) {
    if (thread) {
        __thread_remove_thread_from_queue((thread_t **)(&(get_current_task()->threads)), thread);
        kfree(thread);
    }
}

/**
 * @brief Thread scheduler
 * @note : Check if a thread is ready to be added to the ready queue
 *       If so, add it to the ready queue
 *      Else, continue
 * @note : This function is called by the scheduler
 */
void thread_schedule(thread_t *list) {
    thread_t *tmp = list;

    while (tmp) {
        if (tmp->state == THREAD_WAITING) {
            tmp->state = THREAD_RUNNING;
            thread_switch_context(tmp);
            break;
        }
        tmp = tmp->next;
    }
}

// void thread_start(thread_t *thread) {
//     thread->state = THREAD_RUNNING;
// }

// void thread_stop(thread_t *thread) {
//     thread->state = THREAD_STOPPED;
// }

void thread_sleep(__unused__ uint32_t ms) {
    thread_t *tmp = get_current_task()->threads;

    while (tmp) {
        if (tmp == get_current_task()->threads) {
            tmp->state = THREAD_SLEEPING;
            // tmp->wake_up_time = get_system_tick() + ms;
            // Wake Up in scheduler
            break;
        }
        tmp = tmp->next;
    }
}

void thread_switch_context(thread_t *thread) {

    if (thread == NULL) {
        __WARN_NO_RETURN("Thread is NULL");
    }

    printk("Switching to thread "_GREEN
           "[%d]"_END
           "\n",
           thread->tid);

    printk("Current Thread "_GREEN
           "[%d]"_END
           " ESP: "_GREEN
           "0x%x"_END
           ", EBP: "_GREEN
           "0x%x"_END
           ", EIP: "_GREEN
           "0x%x"_END
           "\n",
           current_thread->tid, current_thread->esp, current_thread->ebp, current_thread->eip);

    uint32_t ebp, esp, eip;

    __asm__ volatile(
        "cli\n\t"           // Disable interrupts
        "pushf\n\t"         // Save flags
        "pusha\n\t"         // Save general registers
        "mov %%esp, %0\n\t" // Save ESP
        "mov %%ebp, %1\n\t" // Save EBP
        : "=r"(esp), "=m"(ebp)
        :
        : "memory");


    eip = read_eip();

    current_thread->esp = esp;
    current_thread->ebp = ebp;
    current_thread->eip = eip;

    current_thread = thread->next; // Update the current thread
    if (!current_thread)
        current_thread = thread;

    esp = current_thread->esp;
    ebp = current_thread->ebp;
    eip = current_thread->eip;
    
    printk("Next Thread "_GREEN
           "[%d]"_END
           " ESP: "_GREEN
           "0x%x"_END
           ", EBP: "_GREEN
           "0x%x"_END
           ", EIP: "_GREEN
           "0x%x"_END
           "\n",
           current_thread->tid, current_thread->esp, current_thread->ebp, current_thread->eip);

    __asm__ volatile(
        "mov %0, %%esp\n\t"
        "mov %1, %%ebp\n\t"
        "mov %2, %%eax\n\t" // Load the thread function's address into EAX
        "popa\n\t"
        "popf\n\t"
        "sti\n\t"
        "jmp *%%eax\n\t" // Jump to the thread function's address
        :
        : "m"(current_thread->esp), "m"(current_thread->ebp), "m"(current_thread->eip)
        : "eax", "memory");
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 UTILS - DISPLAY                                ||
// ! ||--------------------------------------------------------------------------------||

void thread_display_threads(thread_t *list) {
    thread_t *tmp = list;

    if (!tmp) {
        printk("No threads\n");
    } else {
        while (tmp) {
            printk("Thread "_GREEN
                   "[%d]"_END
                   "\n",
                   tmp->tid);
            tmp = tmp->next;
        }
    }
}