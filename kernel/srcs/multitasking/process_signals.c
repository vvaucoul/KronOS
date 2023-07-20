/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_signals.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 22:32:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 00:18:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>

// Fonction : task_add_signal
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

// Fonction : task_remove_signal
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
            // Supprimer le signal de la file
            signal_node_t *signal = current->next;
            current->next = current->next->next;
            kfree(signal);
            return;
        }
        current = current->next;
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     HANDLER                                    ||
// ! ||--------------------------------------------------------------------------------||

void __signal_handler(task_t *current_task) {
    if (current_task->signal_queue == NULL) {
        return;
    }

    signal_node_t *signal = current_task->signal_queue;
    current_task->signal_queue = signal->next;

    if (signal->handler != NULL) {
        signal->handler(signal->signum);
    }

    // Libérer la mémoire allouée pour la structure de signal
    task_remove_signal(current_task, signal->signum);
}