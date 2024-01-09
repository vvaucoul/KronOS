/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   semaphore.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 12:07:06 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 13:25:51 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/semaphore.h>

void semaphore_down(semaphore_t *semaphore) {
    ASM_CLI(); // Start of critical section
    while (semaphore->value <= 0) {
        // This is a simple busy-wait loop, which is not efficient.
        // In a real system, you would want to block the task and switch to another one.
    }
    semaphore->value--;
    ASM_STI(); // End of critical section
}

void semaphore_up(semaphore_t *semaphore) {
    ASM_CLI(); // Start of critical section
    semaphore->value++;
    // If there are tasks waiting on this semaphore, wake one of them up.
    if (semaphore->wait_queue) {
        // Remove a task from the wait queue and wake it up.
        // You would need to add code here to manage the wait queue.
    }
    ASM_STI(); // End of critical section
}

int sem_init(semaphore_t *semaphore, int pshared, unsigned int valeur) {
    __UNUSED(pshared);
    semaphore->value = valeur;
    semaphore->wait_queue = NULL;
    return 0;
    
}

int sem_wait(semaphore_t *semaphore) {
    __UNUSED(semaphore);
    return 0;
}

int sem_trywait(semaphore_t *semaphore) {
    __UNUSED(semaphore);
    return 0;
}

int sem_post(semaphore_t *semaphore) {
    __UNUSED(semaphore);
    return 0;
}

int sem_getvalue(semaphore_t *semaphore, int *sval) {
    __UNUSED(semaphore);
    __UNUSED(sval);
    return 0;
}

int sem_destroy(semaphore_t *semaphore) {
    __UNUSED(semaphore);
    return 0;
}