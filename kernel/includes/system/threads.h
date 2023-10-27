/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/26 17:45:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 11:01:26 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef THREADS_H
#define THREADS_H

#include <kernel.h>

/**
 * Threads
 *
 * Threads for process
 */

#define THREADS_MAX 256
#define THREADS_MAX_BY_PROCESS 16

typedef enum e_thread_state {
    THREAD_RUNNING,
    THREAD_SLEEPING,
    THREAD_WAITING,
    THREAD_STOPPED,
} thread_state_t;

typedef struct s_thread {
    uint32_t esp; // Stack pointer
    uint32_t ebp; // Base pointer
    uint32_t eip; // Instruction pointer

    void *func; // Function to execute

    uint32_t tid;         // Thread ID
    thread_state_t state; // Thread state

    struct s_thread *next; // Next thread

    uint32_t wake_up_time; // Wake up time
} thread_t;

extern thread_t *current_thread;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    FUNCTIONS                                   ||
// ! ||--------------------------------------------------------------------------------||

extern void thread_init(void);

extern thread_t *thread_create(void (*func)(void));
extern void thread_destroy(thread_t *thread);
extern void thread_schedule(thread_t *list);

// extern void thread_start(thread_t *thread);
// extern void thread_stop(thread_t *thread);
extern void thread_sleep(uint32_t ms);

extern void thread_display_threads(thread_t *list);
extern void thread_switch_context(thread_t *thread);

#endif /* !THREADS_H */