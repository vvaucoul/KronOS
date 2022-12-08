/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 12:11:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/08 12:21:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel.h>

typedef enum e_process_state
{
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_ZOMBIE,
    PROCESS_STATE_THREAD,
} process_state_t;

typedef struct s_process
{
    uint32_t pid;
    process_state_t state;
    
    struct s_process *father;
    struct s_process *children;

    

} process_t;

#endif /* !PROCESS_H */