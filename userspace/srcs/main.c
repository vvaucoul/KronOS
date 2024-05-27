/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 20:23:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/24 13:07:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <userspace.h>
#include <syscall/user_syscall.h>

int switch_user_mode_start(void) {
    write("Hello from userspace\n"); // Cause invalid tss type
    while (1) {
        
    }
    return (0);
}
