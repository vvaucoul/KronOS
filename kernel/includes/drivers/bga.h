/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bga.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/09 22:43:13 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 22:47:39 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BGA_H
# define BGA_H

#include <kernel.h>

#define BGA_ADDRESS 0xA0000

extern uint8_t *bga_memory;

#endif /* !BGA_H */