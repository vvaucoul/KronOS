/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:33:26 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/12 14:10:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <multitasking/process.h>

typedef struct s_cpu_state
{
    process_context_t *scheduler;
    process_t *current_process;
} cpu_state_t;

void init_cpu_state(void);

extern void init_scheduler(void);
void scheduler(uint32_t ebp, uint32_t esp);

extern void scheduler_handler(void);

extern void context_switch(uint32_t esp, uint32_t task);
extern void context_switch_jmp(uint32_t esp, uint32_t task);

#endif /* !SCHEDULER_H */