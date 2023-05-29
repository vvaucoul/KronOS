/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/05/29 14:22:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <multitasking/process.h>
#include <memory/memory.h>

#define STACK_SIZE 4096
#define HEAP_SIZE 4096
#define MAX_PROCESSES 4

#define __E_STACK_CORRUPTED "ERROR: Stack corrupted: \n- %s, line: %d"
#define __E_HEAP_CORRUPTED "ERROR: Heap corrupted: \n- %s, line: %d"
#define __E_PROCESS_CREATION "ERROR: No more processes can be created: \n- %s, line: %d"
#define __E_NO_PROCESS "ERROR: No process to run: \n- %s, line: %d"
#define __ERROR(msg) printk(_RED msg _END "\n", __FILE__, __LINE__)

#define ERROR_NO_MORE_PROCESSES 1
#define ERROR_STACK_ALLOC_FAILED 2
#define ERROR_HEAP_ALLOC_FAILED 3

// Define process states
typedef enum
{
    PROCESS_RUNNING,
    PROCESS_READY,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

// Define process control block (PCB) structure
typedef struct pcb
{
    uint32_t pid;
    int state;
    struct pcb *parent;
    struct pcb *children;
    void *stack_pointer;
    void *heap_pointer;
    int owner_id;
} pcb_t;

// Define global variables
static pcb_t *current_process = NULL;

static pcb_t scheduler_queue[MAX_PROCESSES];

extern bool scheduler_initialized;
static uint32_t queue_start = 0;
static uint32_t queue_end = 0;

// Define system call interface
int create_process(void (*entry_point)(void));
void delete_process(uint32_t pid);
void block_process(uint32_t pid);
void unblock_process(uint32_t pid);

// Define scheduler function
void scheduler(void);

// Define scheduler initialization function
void init_scheduler(void);

// Define context switching function in assembly
extern void switch_context(pcb_t **old_process, pcb_t *new_process);

#endif /* !SCHEDULER_H */