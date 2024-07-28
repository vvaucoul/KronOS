/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_process_threads.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/26 20:42:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 21:59:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/mutex.h>
#include <system/semaphore.h>
#include <system/threads.h>

#include <multitasking/process.h>
#include <workflows/workflows.h>

#include <macros.h>

__unused__ static void __thread_01(void) {
    printk("\t - Thread [01]\n");
    while (1) {
    }
}

__unused__ static void __thread_02(void) {
    printk("\t - Thread [02]\n");
}

__unused__ static void __thread_03(void) {
    printk("\t - Thread [03]\n");
}

__unused__ static void __thread_04(void) {
    printk("\t - Thread [04]\n");
}

static void create_threads() {
    printk("Creating threads\n");

    thread_t *thread = thread_create(__thread_01);
    printk("Thread created\n");

    if (!thread) {
        __THROW_NO_RETURN("Failed to create thread");
    } else {
        printk("Starting thread\n");

        // thread_start(thread);
    }

    while (1) {
        ksleep(1);
    }
}

void threads_test(void) {
    __WORKFLOW_HEADER();

    init_task(create_threads);
    // create_threads();

    while (1) {

        ksleep(1);
    }

    __WORKFLOW_FOOTER();
}