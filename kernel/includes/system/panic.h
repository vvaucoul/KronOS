/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   panic.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/09 12:26:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/09 12:28:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PANIC_H
#define PANIC_H

#include <kernel.h>

extern void kernel_panic(const char *str);

#endif /* PANIC_H */ 