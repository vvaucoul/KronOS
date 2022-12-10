/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_process.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 13:04:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 22:30:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <workflows/workflows.h>
#include <multitasking/scheduler.h>
#include <system/pit.h>
#include <system/panic.h>

static void process_a(void)
{
    static int i = 0;
    while (1)
    {
        printk("Process A : %d\n", i);
        ksleep(1);

        if (i >= 3)
            break;
    }
}

void process_test(void)
{
    __WORKFLOW_HEADER();

    load_binary((uint8_t *)(&process_a), 1024);
    
    ksleep(1);
    __WORKFLOW_FOOTER();
}