/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   smp.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/30 15:33:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 15:52:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*

#define CPUID_SMEP 7
#define CPUID_SMAP 20
#define CPU_CR4_SMEP_BIT 20

#define CPU_CR4_SMAP_BIT 21

void supervisor_memory_protection_init(void) {
    uint32_t eax, ebx, ecx, edx;
    eax = 7;
    ecx = 0;
    cpuid(&eax, &ebx, &ecx, &edx);
    if (ebx & CPUID_SMEP) {
        cpu_cr4_set_bit(CPU_CR4_SMEP_BIT);
        log(Log_Info, "SMEP Enabled");
    }

    if (ebx & CPUID_SMAP) {
        cpu_cr4_set_bit(CPU_CR4_SMAP_BIT);
        log(Log_Info, "SMAP Enabled");
    }
}

*/

#include <kernel.h>
#include <memory/smp.h>
#include <asm/asm.h>

/*
** SMP: Supervisor memory protection
*/

void *user_memcpy(void *destination, const void *source, size_t size)
{
    // Disable SMAP protections.
    SMP_SET_AC();
    // Perform normal memcpy.
    void *ret = kmemcpy(destination, source, size);
    // Restore SMAP protections.
    SMP_CLEAR_AC();
    return ret;
}

static void cpuid(uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    // Perform CPUID instruction.
    __asm__ volatile("cpuid"
                     : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
                     : "a"(*eax), "c"(*ecx));
}

static void __smp_init(void)
{
    uint32_t eax = 0x07;
    uint32_t ebx;
    uint32_t ecx = 0;
    uint32_t edx;

    cpuid(&eax, &ebx, &ecx, &edx);
    if (ebx & CPUID_SMEP)
    {
        SET_CR4(CPU_CR4_SMEP_BIT);
        ksh_log_info("LOG", "SMEP Enabled");
    }
    if (ebx & CPUID_SMAP)
    {
        SET_CR4(CPU_CR4_SMAP_BIT);
        ksh_log_info("LOG", "SMAP Enabled");
    }
}

void smp_init(void)
{
    __smp_init();
}