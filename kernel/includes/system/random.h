/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:03:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/23 13:35:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAND_H
#define RAND_H

#include <kernel.h>

#define RA 48271
#define RC 0
#define RM 2147483647

extern void random_init();
extern uint32_t rand();
extern void srand(uint32_t new_seed);

#endif /* !RAND_H */