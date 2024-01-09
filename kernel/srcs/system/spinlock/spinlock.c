/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spinlock.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 23:18:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 12:08:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/spinlock.h>

/**
 * Spinlock
 * - A spinlock is a lock which causes a thread trying to acquire it to simply wait in a loop ("spin")
 */
void spinlock_acquire(spinlock_t *lock) {
    while (*lock || __sync_lock_test_and_set(lock, 1)) {
        // Spin
    }
}

/**
 * Spinlock release
 * - Release a spinlock
 */
void spinlock_release(spinlock_t *lock) {
    *lock = 0;
}