/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bits.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/15 14:22:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/15 14:26:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _BITS_H
#define _BITS_H

/*******************************************************************************
 *                             BITWISE OPERATIONS                              *
 ******************************************************************************/

typedef unsigned char byte;

#define BIT(x) (1 << (x))
#define BIT_SET(x, y) ((x) |= (y))
#define BIT_CLEAR(x, y) ((x) &= ~(y))
#define BIT_FLIP(x, y) ((x) ^= (y))
#define BIT_CHECK(x, y) ((x) & (y))

#define BIT_SET_ALL(x) ((x) = ~0)
#define BIT_CLEAR_ALL(x) ((x) = 0)
#define BIT_FLIP_ALL(x) ((x) = ~(x))
#define BIT_CHECK_ALL(x) ((x) == ~0)

#define BITWISE_LEFT_SHIFT(x, y) ((x) << (y))
#define BITWISE_RIGHT_SHIFT(x, y) ((x) >> (y))

#endif /* _BITS_H */