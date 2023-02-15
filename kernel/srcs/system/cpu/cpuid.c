/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cpuid.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/10 13:11:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/15 12:16:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cpu.h>

bool __cpuid_available = false;

char cpu_vendor[CPU_INFOS_SIZE];
uint32_t cpu_family;
uint32_t cpu_model;

char hypervisor[CPU_INFOS_SIZE];

static int get_model(void)
{
    int ebx, unused;

    __cpuid(0, unused, ebx, unused, unused);
    return (ebx);
}

void get_cpu_version()
{
    unsigned int eax, ebx, ecx, edx;

    // Call __cpuid with an input value of 1 to get the CPU version
    __cpuid(1, eax, ebx, ecx, edx);

    // Extract the family and model information from the lower 16 bits of the eax register
    unsigned int family = (eax & 0x0FFF0000) >> 16;
    unsigned int model = (eax & 0x0000FFF0) >> 4;

    // Extract the stepping and reserved information from the upper 16 bits of the eax register
    unsigned int stepping = eax & 0x0000000F;
    unsigned int reserved = (eax & 0xF0000000) >> 28;

    // Print the CPU version information
    // printk("CPU version: family %u, model %u, stepping %u, reserved %u\n", family, model, stepping, reserved);

    __UNUSED(family);
    __UNUSED(model);
    __UNUSED(stepping);
    __UNUSED(reserved);
}

static void get_cpuid_vendor(char *vendor)
{
    uint32_t eax, ebx, ecx, edx;
    char vendor_string[13];

    __cpuid(0, eax, ebx, ecx, edx);
    memcpy(vendor_string, &ebx, sizeof(uint32_t));
    memcpy(vendor_string + 4, &edx, sizeof(uint32_t));
    memcpy(vendor_string + 8, &ecx, sizeof(uint32_t));
    vendor_string[12] = '\0';

    strcpy(vendor, vendor_string);
}

static void get_cpuid_hypervisor(char *hypervisor)
{
    uint32_t eax, ebx, ecx, edx;
    char hypervisor_string[13];

    __cpuid(0x40000000, eax, ebx, ecx, edx);
    memcpy(hypervisor_string, &ebx, sizeof(uint32_t));
    memcpy(hypervisor_string + 4, &edx, sizeof(uint32_t));
    memcpy(hypervisor_string + 8, &ecx, sizeof(uint32_t));
    hypervisor_string[12] = '\0';

    strcpy(hypervisor, hypervisor_string);
}

static int check_apic(cpu_info_t info)
{
    uint32_t eax, unused, ebx;

    __get_cpuid(1, &eax, &unused, &unused, &ebx);
    return (ebx & (info));
}

void get_cpu_informations(void)
{
    printk("\t   CPUID: ["_GREEN
           "%s"_END
           "]\n",
           __cpuid_available == true ? "Available" : "Not Available");
    if (__cpuid_available == false)
        return;
    printk("\t   Vendor: ["_GREEN
           "%s"_END
           "]\n",
           cpu_vendor);
    printk("\t   Hypervisor: ["_GREEN
           "%s"_END
           "]\n",
           hypervisor);
}

bool init_cpuid(void)
{
    if (cpu_availability() == 0 || check_apic(CPUID_APIC) == 0)
    {
        if (__DISPLAY_INIT_LOG__)
            printk(_YELLOW "[%s] " _END "- " _GREEN "[INIT] " _RED "%s " _END "\n", "CPUID", "CPUID not supported");
        return (false);
    }
    else
    {
        bzero(cpu_vendor, CPU_INFOS_SIZE);

        cpu_family = 0;
        cpu_model = 0;
        bzero(hypervisor, CPU_INFOS_SIZE);

        get_cpuid_vendor(cpu_vendor);
        get_cpuid_hypervisor(hypervisor);
        get_cpu_version();
        cpu_model = get_model();

        return (true);
    }
}