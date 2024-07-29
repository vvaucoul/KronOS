/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topology.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 00:52:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/30 01:13:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cpu.h>

// TMP
#include <system/pit.h>

#include <cpuid.h>

cpu_topology_t cpu_topology;

/* Vérifier si CPUID est supporté (avec flag HTT) */
static bool cpuid_is_supported(void) {
    uint32_t eax, ebx, ecx, edx;
    uint32_t htt = 0;

    __cpuid(0x00000001, eax, ebx, ecx, edx);
    htt = (edx >> 28) & 1;

    if (htt == 1)
        printk("\t\t "_YELLOW "[CID]"_END " - "_GREEN "%s" _END "\n", "[SUPPORTED]");
    else
        printk("\t\t "_YELLOW "[CID]"_END " - "_RED "%s" _END "\n", "[NOT SUPPORTED]");

    return (htt == 1);
}

/* Obtenir la topologie du CPU */
void get_cpu_topology(void) {
    // Vérifier si CPUID est supporté
    if (!cpuid_is_supported())
        return;

    __cpuid_available = true;
    memset(&cpu_topology, 0, sizeof(cpu_topology));

    uint32_t cpuInfo[4] = {0, 0, 0, 0};
    uint32_t nExIds, i;

    // Obtenir le nombre d'IDs étendus
    __cpuid(0x80000000, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    nExIds = cpuInfo[0];

    // Lire les informations de la marque du CPU
    for (i = 0x80000000; i <= nExIds; ++i) {
        __cpuid(i, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);

        if (i == 0x80000002)
            memcpy(cpu_topology.brandString, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000003)
            memcpy(cpu_topology.brandString + 16, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000004)
            memcpy(cpu_topology.brandString + 32, cpuInfo, sizeof(cpuInfo));
    }

    // Obtenir le nombre de cores physiques par package
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.physicalCoresPerPackage = (cpuInfo[0] >> 26) + 1;

    // Obtenir le nombre de cores logiques par core physique
    __cpuid(0x00000001, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.logicalCoresPerPhysicalCore = ((cpuInfo[1] >> 16) & 0xff);

    // Obtenir le nombre de sockets
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.socketCount = ((cpuInfo[0] >> 14) & 0xfff) + 1;

    // Obtenir le nombre de cores
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.coreCount = ((cpuInfo[0] >> 26) & 0x3f) + 1;

    // Obtenir le nombre de threads
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.threadCount = ((cpuInfo[0] >> 14) & 0xfff) + 1;

    // Obtenir la taille du cache L1
    __cpuid(0x00000005, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l1CacheSize = cpuInfo[2] * 1024;

    // Obtenir la taille du cache L2
    __cpuid(0x00000006, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l2CacheSize = ((cpuInfo[2] >> 16) & 0xffff) * 1024;

    // Obtenir la taille du cache L3
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l3CacheSize = (((cpuInfo[2] >> 22) + 1) * ((cpuInfo[2] & 0x3ff) + 1) * ((cpuInfo[2] >> 12) & 0x3ff) * (cpuInfo[1] & 0xff)) * 64;

    // Obtenir la fréquence actuelle du CPU
    cpu_topology.currentFrequency = get_cpu_frequency();
    printk("\t\t\t   -"_GREEN " FREQUENCY: " _END "%d MHz\n", cpu_topology.currentFrequency);

    // Afficher les informations du CPU
    printk(_END "\t\t\t   -"_GREEN " VENDOR: " _END "%s" _END "\n", cpu_vendor);
    printk(_END "\t\t\t   -"_GREEN " HYPERVISOR: " _END "%s" _END "\n", hypervisor);
}