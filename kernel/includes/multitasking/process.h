/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:07:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/16 23:36:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel.h>
#include <memory/paging.h>

#define __LOG_HEADER ""_END                                         \
                     "- "_YELLOW                                    \
                     "[MULTITASKING]" _END " - " _GREEN "[%s] "_END \
                     ": "_CYAN                                      \
                     "%s\n" _END

#define __LOG(x, ...) printk(x, ##__VA_ARGS__)

#define MAX_PROCESS 16 // Get max process with CPU

#define PROCESS_KERNEL_STACK (0x1000 / 4)
#define PROCESS_USER_STACK 0x1000

#define PROCESS_NULL_SEGMENT 0x00
#define PROCESS_KERNEL_CODE_SEGMENT 0x01
#define PROCESS_KERNEL_DATA_SEGMENT 0x02
#define PROCESS_KERNEL_STACK_SEGMENT 0x03
#define PROCESS_KERNEL_TSS_SEGMENT 0x04
#define PROCESS_USER_CODE_SEGMENT 0x05
#define PROCESS_USER_DATA_SEGMENT 0x06
#define PROCESS_USER_STACK_SEGMENT 0x07
#define PROCESS_USER_TSS_SEGMENT 0x08

#define PROCESS_NULL_SELECTOR 0x00
#define PROCESS_KERNEL_CODE_SELECTOR 0x08
#define PROCESS_KERNEL_DATA_SELECTOR 0x10
#define PROCESS_KERNEL_STACK_SELECTOR 0x18
#define PROCESS_KERNEL_TSS_SELECTOR 0x38
#define PROCESS_USER_CODE_SELECTOR 0x23
#define PROCESS_USER_DATA_SELECTOR 0x2B
#define PROCESS_USER_STACK_SELECTOR 0x33
#define PROCESS_USER_TSS_SELECTOR 0x3B

typedef enum e_process_level
{
    PROCESS_LEVEL_KERNEL,
    PROCESS_LEVEL_USER
} process_level_t;

typedef enum e_process_state
{
    PROCESS_STATE_UNUSED,
    PROCESS_STATE_INITING,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_ZOMBIE,
    PROCESS_STATE_THREAD,
    PROCESS_STATE_TERMINATED
} process_state_t;

#define __PROCESS_NAME_LEN 32

typedef struct s_process
{
    char name[__PROCESS_NAME_LEN];

    uint32_t pid;
    uint32_t owner;

    struct regs *context;
    process_state_t state;

    uint32_t kernel_stack;
    uint32_t user_stack;

    uint32_t kernel_esp;
    uint32_t user_esp;

    page_t *page;
    page_directory_t *page_directory;

    struct s_process *parent;
    struct s_process *child;

    uint32_t *sig_queue;

    void *fn; // tmp should be removed
} process_t;

extern process_t process_table[MAX_PROCESS];

extern void init_process();

extern process_t *create_processus(const char *name, struct regs *cpu_state, void *kernel_stack, void (*entry_point)(void), process_level_t level, uint32_t size);

extern void destroy_processus(process_t *process);

#endif /* !PROCESS_H */