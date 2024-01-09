/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphore.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 12:06:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 12:07:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <kernel.h>
#include <multitasking/process.h>

typedef struct {
    int32_t value;      // Current value of the semaphore
    task_t *wait_queue; // Queue of tasks waiting on the semaphore
} semaphore_t;

#endif /* !SEMAPHORE_H */