/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:07:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/01 14:07:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel.h>
#include <memory/paging.h>

#define KERNEL_STACK_SIZE 2048 // 2KB

typedef struct s_task {
    int32_t pid;  // Process id
    int32_t ppid; // Parent pid

    uint32_t esp, ebp; // Stack and base pointer
    uint32_t eip;      // Instruction pointer

    page_directory_t *page_directory; // Page directory

    uint32_t kernel_stack; // Kernel stack
    struct s_task *next;   // Next task
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

void switch_to_user_mode(void);

extern uint32_t read_eip(void);

#endif /* !PROCESS_H */