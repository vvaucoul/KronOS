/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 18:37:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/02 14:49:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
# define KERNEL_H

#pragma once

#include "../../libkfs/includes/libkfs.h"

#define __DISPLAY_INIT_LOG__ true
#define __HIGHER_HALF_KERNEL__ true

#define _KERNEL_MEMORY_START 0xC0000000

#endif /* KERNEL_H */