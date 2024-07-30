/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   random.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:03:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 11:53:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RAND_H
#define RAND_H

#include <stdint.h>

#define RNG_A 1103515245
#define RNG_C 12345
#define RNG_M 0x80000000 // 2^31

extern void rand_init();
extern uint32_t rand(void);
extern void srand(uint32_t new_seed);

#endif /* !RAND_H */