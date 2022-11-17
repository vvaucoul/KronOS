/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:37:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/17 02:05:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
# define KERNEL_H

#pragma once

#include "../../libkfs/includes/libkfs.h"

#define __DISPLAY_INIT_LOG__ true
#define __HIGHER_HALF_KERNEL__ false

extern void kernel_log_info(const char *part, const char *name);

#endif /* KERNEL_H */