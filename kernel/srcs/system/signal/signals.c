/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 10:10:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>
#include <multitasking/scheduler.h>
#include <system/signal.h>

signal_node_t signals[SIGNALS_COUNT];

void signal(pid_t pid, int signum) {
    task_t *task = get_task(pid);

    if (task) {
        if (signum >= 0 && signum < SIGNALS_COUNT) {
            task_add_signal(task, signum, signals[signum].handler);
        } else {
            __THROW_NO_RETURN("Signal not found (maybe not implemented)");
        }
    } else {
        __THROW_NO_RETURN("Task not found");
    }
}

void sigchld_handler() {
    
}

void kill_handler(int32_t signum) {
    switch (signum) {
    case SIGKILL: {
        printk(_YELLOW "SIGNAL Killing task "_GREEN
                       "[%d]"_END
                       "\n",
               getpid());
        get_current_task()->exit_code = 0;
        kill_task(getpid());
        break;
    }
    default:
        __THROW_NO_RETURN("Signal not found");
        break;
    }
}

static void __add_signal_handler(int signum, void (*handler)(int32_t), char *name) {

    printk("\t\t\t   - Signal " _YELLOW "[%d]" _END " - " _GREEN "%s" _END "\n", signum, name);

    signals[signum] = (signal_node_t){
        .name = name,
        .signum = signum,
        .handler = handler,
        .next = NULL,
    };

    // signal_node_t *new_signal = (signal_node_t *)kmalloc(sizeof(signal_node_t));
    // new_signal->signum = signum;
    // new_signal->handler = handler;
    // new_signal->next = NULL;

    // if (__signal_count == 0) {
    //     __signal_handlers = new_signal;
    // } else {
    //     signal_node_t *current_signal = __signal_handlers;
    //     while (current_signal->next != NULL) {
    //         current_signal = current_signal->next;
    //     }
    //     current_signal->next = new_signal;
    // }
    // __signal_count++;
}

void init_signals(void) {
    bzero((uint8_t *)signals, sizeof(signal_node_t) * SIGNALS_COUNT);
    __add_signal_handler(SIGKILL, kill_handler, STRINGIFY(SIGKILL));
    // __add_signal_handler(SIGCHLD, sigchld_handler, STRINGIFY(SIGCHLD));
}