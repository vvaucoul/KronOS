/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rand.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:03:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:13:14 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/random.h>
#include <system/time.h>

#define A 48271
#define C 0
#define M 2147483647

static unsigned int seed; // No initial value

void srand_from_time() {
    tm_t date = gettime();
    // Combine the current time into a single number
    seed = date.year * 10000000000 + date.month * 100000000 + date.day * 1000000 + date.hours * 10000 + date.minutes * 100 + date.seconds;
}

unsigned int rand() {
    seed = (A * seed + C) % M;
    return seed;
}