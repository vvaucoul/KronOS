/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rdtsc.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 19:54:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/06/01 16:12:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cpu.h>
#include <system/pit.h>

#include <asm/asm.h>

/*
** RDTSC - Read Time Stamp Counter
** Goal: Get the number of cycles since the last reset
*/

/* Pour lire le TSC */
inline uint64_t rdtsc() {
    uint64_t a, d;
    __asm__ volatile("rdtsc"
                     : "=a"(a), "=d"(d));
    return ((uint128_t)d << 32) | a;
}

/* Pour obtenir la fréquence du CPU en MHz */
double get_cpu_frequency() {
    uint64_t start, end;
    kernel_log_info("CPU", "Calculating CPU Frequency...");

    ASM_STI();
    start = rdtsc();
    ksleep(1);
    end = rdtsc();
    ASM_CLI();

    return (end - start) / 1e6; /* Pour convertir en MHz */
}