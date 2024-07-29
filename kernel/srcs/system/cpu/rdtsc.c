/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rdtsc.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/27 19:54:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 01:11:32 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <kernel.h>
#include <system/cpu.h>
#include <system/pit.h>
#include <asm/asm.h>

#include <stdint.h>

/*
** RDTSC - Read Time Stamp Counter
** Goal: Get the number of cycles since the last reset
*/

/**
 * @brief Returns the value of the Time Stamp Counter (TSC) register.
 *
 * @return The value of the TSC register.
 */
static inline uint64_t rdtsc(void) {
    uint32_t a, d;
    __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
    return ((uint64_t)d << 32) | a;
}

/**
 * @brief Retrieves the CPU frequency.
 *
 * This function returns the frequency of the CPU in Hertz.
 *
 * @return The CPU frequency in Hertz.
 */
double get_cpu_frequency(void) {
    uint64_t start, end;
    kernel_log_info("CPU", "Calculating CPU Frequency...");

    ASM_STI();
    start = rdtsc();
    ksleep(1); // Dormir pendant 1 seconde pour mesurer les cycles
    end = rdtsc();
    ASM_CLI();

    return (end - start) / 1e6; // Convertir en MHz
}