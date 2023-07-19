/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 09:59:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 10:12:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_H
#define SIGNAL_H

typedef enum {
    SIGINT,  // Interrupt signal, often sent by Ctrl+C
    SIGKILL, // Kill signal, which immediately terminates the task
    SIGSTOP, // Stop signal, which pauses the task
    SIGCONT, // Continue signal, which resumes a paused task
    // More signals ...
} signal_t;

typedef struct signal_node {
    signal_t signal;
    struct signal_node* next;
} signal_node_t;

#endif /* !SIGNAL_H */