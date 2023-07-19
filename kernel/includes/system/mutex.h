/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mutex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 11:35:41 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 13:21:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MUTEX_H
#define MUTEX_H

#include <kernel.h>

typedef enum {
    MUTEX_UNLOCKED = 0,
    MUTEX_LOCKED = 1
} mutex_state_t;

typedef struct {
    mutex_state_t state; // Mutex state, 0 = unlocked, 1 = locked
    uint32_t owner;      // PID of the owner of the mutex, if it is locked
    task_t *wait_queue;  // Queue of tasks waiting on the mutex
} mutex_t;

void init_mutex(mutex_t *mutex);
void acquire_mutex(mutex_t *mutex);
void release_mutex(mutex_t *mutex);

// ! ||--------------------------------------------------------------------------------||
// ! ||                    Alias for acquire_mutex and release_mutex                   ||
// ! ||--------------------------------------------------------------------------------||

#define mutex_lock(mutex) acquire_mutex(mutex)
#define mutex_unlock(mutex) release_mutex(mutex)

#endif /* !MUTEX_H */