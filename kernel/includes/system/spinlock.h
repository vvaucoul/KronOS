/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   spinlock.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/21 23:19:27 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/21 23:22:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef volatile int spinlock_t;

extern void spinlock_acquire(spinlock_t *lock);
extern void spinlock_release(spinlock_t *lock);

#endif /* !SPINLOCK_H */