/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rand.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 14:03:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:53:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/random.h>
#include <system/time.h>

static unsigned int seed; // No initial value

void random_init() {
    tm_t date = gettime();

    // Combine the current time into a single number
    seed = date.year * 10000000000 + date.month * 100000000 + date.day * 1000000 + date.hours * 10000 + date.minutes * 100 + date.seconds;
}

uint32_t rand() {
    seed = (RA * seed + RC) % RM;
    return seed;
}