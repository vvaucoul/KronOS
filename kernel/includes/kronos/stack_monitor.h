/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_monitor.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 13:02:50 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 14:06:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STACK_MONITOR_H
#define STACK_MONITOR_H

#include <stdint.h>

/**
 * This file contains the declaration of the stack_monitor module, which provides
 * functionality for monitoring the stack.
 */

extern uint32_t sm_get_stack_usage(void);
extern uint32_t sm_get_stack_usage_percentage(void);
extern uint32_t sm_get_stack_size(void);
extern uint32_t sm_get_stack_base(void);
extern uint32_t sm_get_stack_top(void);
extern uint32_t sm_get_stack_marker(void);

extern void sm_print_stack_info(void);

#endif /* !STACK_MONITOR_H */