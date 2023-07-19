/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:07:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 22:37:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel.h>
#include <memory/paging.h>

#include <system/signal.h>

#define KERNEL_STACK_SIZE 8192 // 2KB

typedef enum e_task_state {
    TASK_RUNNING,
    TASK_SLEEPING,
    TASK_WAITING,
    TASK_STOPPED,
    TASK_ZOMBIE,
    TASK_THREAD,
} task_state_t;

typedef struct s_task {
    int32_t pid;  // Process id
    int32_t ppid; // Parent pid

    uint32_t owner; // Owner id (user id)

    uint32_t esp, ebp; // Stack and base pointer
    uint32_t eip;      // Instruction pointer

    page_directory_t *page_directory; // Page directory

    uint32_t kernel_stack;      // Kernel stack
    struct s_task *next, *prev; // Next and previous task

    uint32_t exit_code;

    task_state_t state;

    signal_node_t *signal_queue; // Queue of signals to be processed
} task_t;

void init_tasking(void);
void switch_task(void);

int32_t task_fork(void);

int32_t getpid(void);
int32_t getppid(void);

task_t *get_current_task(void);
task_t *get_task(int32_t pid);

int32_t init_task(void func(void));

int32_t kill_task(int32_t pid);
int32_t wait_task(int32_t pid);

void switch_to_user_mode(void);

extern uint32_t read_eip(void);

void exit_task(uint32_t retval);

__attribute__((pure)) extern page_directory_t *get_task_directory(void);

task_t *get_wait_queue(void);
extern uint32_t getuid(void);

extern void lock_task(task_t *task);
extern void unlock_task(task_t *task);

#endif /* !PROCESS_H */