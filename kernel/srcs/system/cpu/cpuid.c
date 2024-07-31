/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cpuid.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/10 13:11:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/31 16:34:24 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <macros.h>
#include <system/cpu.h>

#include <cpuid.h>
#include <string.h>

bool __cpuid_available = false;

char cpu_vendor[CPU_INFOS_SIZE];
uint32_t cpu_family;
uint32_t cpu_model;

char hypervisor[CPU_INFOS_SIZE];

static void get_cpuid(uint32_t code, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
    __cpuid(code, *a, *b, *c, *d);
}

static void get_cpuid_vendor(char *vendor) {
    uint32_t eax, ebx, ecx, edx;
    char vendor_string[13];

    get_cpuid(0, &eax, &ebx, &ecx, &edx);
    memcpy(vendor_string, &ebx, sizeof(uint32_t));
    memcpy(vendor_string + 4, &edx, sizeof(uint32_t));
    memcpy(vendor_string + 8, &ecx, sizeof(uint32_t));
    vendor_string[12] = '\0';

    strcpy(vendor, vendor_string);
}

static void get_cpuid_hypervisor(char *hypervisor) {
    uint32_t eax, ebx, ecx, edx;
    char hypervisor_string[13];

    get_cpuid(0x40000000, &eax, &ebx, &ecx, &edx);
    memcpy(hypervisor_string, &ebx, sizeof(uint32_t));
    memcpy(hypervisor_string + 4, &edx, sizeof(uint32_t));
    memcpy(hypervisor_string + 8, &ecx, sizeof(uint32_t));
    hypervisor_string[12] = '\0';

    strcpy(hypervisor, hypervisor_string);
}

static int get_model(void) {
    uint32_t eax, ebx, ecx, edx;
    get_cpuid(1, &eax, &ebx, &ecx, &edx);
    return (ebx);
}

void get_cpu_version() {
    uint32_t eax, ebx, ecx, edx;

    // Call cpuid with an input value of 1 to get the CPU version
    get_cpuid(1, &eax, &ebx, &ecx, &edx);

    // Extract the family and model information from the lower 16 bits of the eax register
    uint32_t family = (eax & 0x0FFF0000) >> 16;
    uint32_t model = (eax & 0x0000FFF0) >> 4;

    // Extract the stepping and reserved information from the upper 16 bits of the eax register
    uint32_t stepping = eax & 0x0000000F;
    uint32_t reserved = (eax & 0xF0000000) >> 28;

    // Print the CPU version information
    printk("\t   - CPU version: family %u, model %u, stepping %u, reserved %u\n", family, model, stepping, reserved);
}

static int check_apic(cpu_info_t info) {
    uint32_t eax, unused, ebx;

    get_cpuid(1, &eax, &unused, &unused, &ebx);
    return (ebx & (info));
}

void get_cpu_informations(void) {
    printk("\t   CPUID: ["_GREEN "%s"_END "]\n", __cpuid_available ? "Available" : "Not Available");
    if (!__cpuid_available)
        return;
    printk("\t   Vendor: ["_GREEN "%s"_END "]\n", cpu_vendor);
    printk("\t   Hypervisor: ["_GREEN "%s"_END "]\n", hypervisor);
}

bool init_cpuid(void) {
    if (!cpu_availability() || !check_apic(CPUID_APIC)) {
        if (__DISPLAY_INIT_LOG__)
            printk(_YELLOW "[%s] " _END "- " _GREEN "[INIT] " _RED "%s " _END "\n", "CPUID", "CPUID not supported");
        return false;
    }

    bzero(cpu_vendor, CPU_INFOS_SIZE);
    bzero(hypervisor, CPU_INFOS_SIZE);

    get_cpuid_vendor(cpu_vendor);
    get_cpuid_hypervisor(hypervisor);
    get_cpu_version();
    cpu_model = get_model();

    return true;
}