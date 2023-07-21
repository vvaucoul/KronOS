/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 10:10:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 15:46:30 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>
#include <multitasking/scheduler.h>
#include <system/signal.h>

signal_node_t *__signal_handlers = NULL;

// Todo: Debug Signal send to PID 1 and should not
void signal(pid_t pid, int signum) {
    task_t *task = get_task(pid);

    if (task) {
        signal_node_t *current_signal = __signal_handlers;

        while (current_signal) {
            if (current_signal->signum == signum) {
                task_add_signal(task, signum, current_signal->handler);
                break;
            }
            current_signal = current_signal->next;
        }

        if (!current_signal) {
            __THROW_NO_RETURN("Signal not found (maybe not implemented)");
        }
    } else
        __THROW_NO_RETURN("Task not found");
}

void kill_handler(int32_t signum) {
    printk("Send "_GREEN
           "SIGKILL"_END
           " to Task: [%d]\n",
           getpid());
    switch (signum) {
    case SIGKILL: {
        get_current_task()->exit_code = 0;
        kill_task(getpid());
        break;
    }
    default:
        __THROW_NO_RETURN("Signal not found");
        break;
    }
}

static void __add_signal_handler(int signum, void (*handler)(int32_t)) {
    signal_node_t *new_signal = (signal_node_t *)kmalloc(sizeof(signal_node_t));
    new_signal->signum = signum;
    new_signal->handler = handler;
    new_signal->next = NULL;

    if (__signal_handlers == NULL) {
        __signal_handlers = new_signal;
    } else {
        signal_node_t *current_signal = __signal_handlers;
        while (current_signal->next != NULL) {
            current_signal = current_signal->next;
        }
        current_signal->next = new_signal;
    }
}

void init_signals(void) {
    __add_signal_handler(SIGKILL, kill_handler);
}