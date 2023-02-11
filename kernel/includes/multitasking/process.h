/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 12:11:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 22:30:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel.h>

typedef enum e_process_state : uint8_t
{
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_ZOMBIE,
    PROCESS_STATE_THREAD,
} process_state_t;

typedef uint32_t pid_t;

typedef struct s_process
{
    pid_t pid;
    process_state_t state;

    uint32_t stack; // Stack pointer (ESP)
    uint32_t base; // Base pointer (EBP)

    uint32_t owner_id; // Owner ID
    //Todo: Current Signal
        
    struct s_process *father;
    struct s_process *children;

} process_t;

#endif /* !PROCESS_H */