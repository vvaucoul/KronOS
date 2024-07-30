/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rand.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:03:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 11:53:56 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/random.h>
#include <system/time.h>

// Linear Congruential Generator (LCG).
// The LCG is defined by the following recurrence relation:
//
// X_{n+1} = (a * X_n + c) mod m

static unsigned int seed; // No initial value

void rand_init() {
    tm_t date = gettime();

    // Combine the current time into a single number
    seed = date.year * 10000000000 + date.month * 100000000 + date.day * 1000000 + date.hours * 10000 + date.minutes * 100 + date.seconds;
}

uint32_t rand(void) {
    seed = (RNG_A * seed + RNG_C) % RNG_M;

    return seed;
}

void srand(uint32_t new_seed) {
    seed = new_seed;
}