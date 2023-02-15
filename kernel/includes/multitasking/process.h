/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:07:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 14:54:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel.h>
#include <memory/paging.h>

#define __LOG_HEADER ""_END                                     \
                     "- "_YELLOW                                    \
                     "[MULTITASKING]" _END " - " _GREEN "[%s] "_END \
                     ": "_CYAN                                      \
                     "%s\n" _END

#define __LOG(x, ...) printk(x, ##__VA_ARGS__)

#define MAX_PROCESS 32 // Get max process with CPU

#define PROCESS_STACK 0x1000 // 4Ko (1 PAGE_SIZE)

typedef struct s_process_context
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} __attribute__((packed)) process_context_t;

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

    process_context_t *context;
    process_state_t state;

    uint32_t stack;
    page_t *page;
    page_directory_t *page_directory;

    struct s_process *parent;
    struct s_process *child;

    uint32_t *sig_queue;

    void *fn; // tmp should be removed
} process_t;

extern process_t process_table[MAX_PROCESS];

extern void init_process();

extern void create_processus(void *entry_point, uint32_t size);

#endif /* !PROCESS_H */