/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 10:10:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 22:34:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>
#include <system/signal.h>

signal_node_t *__signal_handlers = NULL;

// Todo: Debug Signal send to PID 1 and should not
void signal(int pid, int signum) {
    task_t *task = get_task(pid);

    if (task) {
        signal_node_t *current_signal = __signal_handlers;

        while (current_signal) {
            if (current_signal->signum == signum) {
                current_signal->handler(signum);
                break;
            }
            current_signal = current_signal->next;
        }

        if (!current_signal) {
            __PANIC("Signal not found");
        }

        if (task->signal_queue == NULL) {
            task->signal_queue = current_signal;
        } else {
            signal_node_t *last_signal = task->signal_queue;
            while (last_signal->next != NULL) {
                last_signal = last_signal->next;
            }
            last_signal->next = current_signal;
        }
    }
}

void kill_handler(int32_t signum) {
    printk("Send "_GREEN
           "SIGKILL"_END
           " to Task dummy pid: %d\n",
           getpid());
    switch (signum) {
    case SIGKILL: {
        get_current_task()->exit_code = 0;
        kill_task(get_current_task()->pid);
        break;
    }
    default:
        __THROW_NO_RETURN("Signal not found");
        break;
    }
}

static void __add_signal_handler(int signum, void (*handler)(int32_t)) {
    if (__signal_handlers == NULL) {
        __signal_handlers = (signal_node_t *)kmalloc(sizeof(signal_node_t));
        __signal_handlers->signum = signum;
        __signal_handlers->handler = handler;
        __signal_handlers->next = NULL;
    } else {
        signal_node_t *new_signal = (signal_node_t *)kmalloc(sizeof(signal_node_t));
        new_signal->signum = signum;
        new_signal->handler = handler;
        new_signal->next = __signal_handlers;
        __signal_handlers = new_signal;
    }
}

void init_signals(void) {
    __add_signal_handler(SIGKILL, kill_handler);
}