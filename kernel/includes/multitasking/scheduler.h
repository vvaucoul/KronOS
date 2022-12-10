/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/08 12:58:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <system/isr.h>
#include <memory/paging.h>

#define UCODE_START (uint32_t)0x00600000 // 6MB

typedef enum e_process_state
{
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_ZOMBIE,
    PROCESS_STATE_THREAD,
} process_state_t;

typedef struct s_process
{
    t_regs regs;                      // CPU registers
    page_directory_t *page_directory; // Page directory
    page_t *process_page;             // Current process page

    uint32_t pid;          // Process ID
    process_state_t state; // Process state (Running, Zombie, Thread)

    uint32_t esp; // Stack pointer
    uint32_t ebp; // Base pointer

    uint32_t owner_id; // Owner ID

    struct s_process *parent; // Parent process
    struct s_process *next;   // Next process in the list (children)
} process_t;

extern process_t *process_queue;
extern process_t *current_process;

extern void load_binary(uint8_t *data, uint32_t size);

#endif /* !SCHEDULER_H */