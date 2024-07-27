/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_signals.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 22:32:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/26 22:44:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>

#include <multitasking/scheduler.h>

#include <stdio.h>

/**
 * @brief Add a signal to a task
 * @param task
 * @param signum
 * @param handler
 *
 * @note : Add signal to a specific task
 * @note : If the signal already exists, nothing will be done
 */
void task_add_signal(task_t *task, int signum, void (*handler)(int)) {
    signal_node_t *new_signal = (signal_node_t *)kmalloc(sizeof(signal_node_t));
    new_signal->signum = signum;
    new_signal->handler = handler;
    new_signal->next = NULL;

    if (task->signal_queue == NULL) {
        task->signal_queue = new_signal;
    } else {
        signal_node_t *current = task->signal_queue;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_signal;
    }
}

/**
 * @brief Remove a signal from a task
 * @param task
 * @param signum
 *
 * @note : Remove a signal from a specific task
 * @note : If the signal is not found, nothing will be done
 */
void task_remove_signal(task_t *task, int signum) {
    if (task->signal_queue == NULL) {
        return;
    }

    if (task->signal_queue->signum == signum) {
        signal_node_t *signal = task->signal_queue;
        task->signal_queue = task->signal_queue->next;
        kfree(signal);
        return;
    }

    signal_node_t *current = task->signal_queue;
    while (current->next != NULL) {
        if (current->next->signum == signum) {
            signal_node_t *signal = current->next;
            current->next = current->next->next;
            kfree(signal);
            return;
        }
        current = current->next;
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                      UTILS                                     ||
// ! ||--------------------------------------------------------------------------------||

void task_print_signals(task_t *task) {
    signal_node_t *current = task->signal_queue;
    printk("Task: [%d] Signals: ", task->pid);
    while (current != NULL) {
        printk("[%d] ", current->signum);
        current = current->next;
    }
    printk("\n");
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     HANDLER                                    ||
// ! ||--------------------------------------------------------------------------------||

/**
 * @brief Handle a signal
 *
 * @param current_task
 * @return void
 *
 * @note : This function must be called by the scheduler only !
 *
 * @see scheduler.c
 */
void __signal_handler(task_t *current_task) {
    if (current_task->signal_queue == NULL) {
        return;
    }

    signal_node_t *signal = current_task->signal_queue;
    current_task->signal_queue = signal->next;

    if (signal->handler != NULL) {
        signal->handler(signal->signum);
    }

    /* Free and remove signal */
    task_remove_signal(current_task, signal->signum);
}