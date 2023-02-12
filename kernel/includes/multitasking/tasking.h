/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tasking.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/10 22:31:55 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/12 13:18:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TASKING_H
#define TASKING_H

#include <kernel.h>

typedef struct
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t eflags;
    uint32_t cr3;
} task_regs_t;

typedef struct s_task
{
    task_regs_t regs;
    struct s_task *next;
} task_t;

extern void tasking_init(void);
extern void task_create(task_t *task, void (*entry)(void), uint32_t flags, uint32_t *page_directory);

extern void yield(void);

#endif /* !TASKING_H */