/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spinlock.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 23:18:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 23:22:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/spinlock.h>

void spinlock_acquire(spinlock_t *lock) {
    while (*lock || __sync_lock_test_and_set(lock, 1)) {
    }
}

void spinlock_release(spinlock_t *lock) {
    *lock = 0;
}