/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topology.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 00:52:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 18:55:21 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Detect cpu topology (Needed for Scheduler (multitasking))
// -> https://wiki.osdev.org/Detecting_CPU_Topology_(80x86)

#include <system/cpu.h>

cpu_topology_t cpu_topology;

/* Check Flags HTT */
static bool cpuid_is_supported(void)
{
    uint32_t eax, ebx, ecx, edx;
    uint32_t htt = 0;

    __cpuid(0x00000001, eax, ebx, ecx, edx);
    htt = (edx >> 28) & 1;

    if (htt == 1)
        printk("\t\t "_YELLOW
               "[CPUID SUPPORTED]"_END
               " - "_GREEN
               "%s" _END "\n",
               "TRUE");
    else
        printk("\t\t "_YELLOW
               "[CPUID SUPPORTED]"_END
               " - "_RED
               "%s" _END "\n",
               "FALSE");

    return (htt == 1);
}

void get_cpu_topology(void)
{
    // CPUID eax = 0x00000001;

    // Check if CPUID is supported (with flag HTT)
    if (cpuid_is_supported() == false)
        return;

    int cpuInfo[4] = {0, 0, 0, 0};
    unsigned nExIds, i = 0;

    // Get the information associated with each extended ID.
    __cpuid(0x80000000, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    nExIds = cpuInfo[0];

    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(i, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);

        // Interpret CPU brand string and cache information.
        if (i == 0x80000002)
            memcpy(cpu_topology.brandString, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000003)
            memcpy(cpu_topology.brandString + 16, cpuInfo, sizeof(cpuInfo));
        else if (i == 0x80000004)
            memcpy(cpu_topology.brandString + 32, cpuInfo, sizeof(cpuInfo));
    }

    // Display all information in a human readable format.
    printk("CPU Brand: %s\n", cpu_topology.brandString);

    // Get the number of physical cores per package.
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.physicalCoresPerPackage = (cpuInfo[0] >> 26) + 1;
    printk("Physical cores per package: %d\n", cpu_topology.physicalCoresPerPackage);

    // Get the number of logical cores per physical core.
    __cpuid(0x00000001, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.logicalCoresPerPhysicalCore = ((cpuInfo[1] >> 16) & 0xff);
    printk("Logical cores per physical core: %d\n", cpu_topology.logicalCoresPerPhysicalCore);

    // Get Socket count
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.socketCount = ((cpuInfo[0] >> 14) & 0xfff) + 1;
    printk("Socket count: %d\n", cpu_topology.socketCount);

    // Get Core Count
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.coreCount = ((cpuInfo[0] >> 26) & 0x3f) + 1;
    printk("Core count: %d\n", cpu_topology.coreCount);

    // Get Thread Count
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.threadCount = ((cpuInfo[0] >> 14) & 0xfff) + 1;
    printk("Thread count: %d\n", cpu_topology.threadCount);

    // Get L1 Cache Size
    __cpuid(0x00000002, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l1CacheSize = ((cpuInfo[2] >> 24) & 0xff) * 1024;
    printk("L1 Cache Size: %d\n", cpu_topology.l1CacheSize);

    // Get L2 Cache Size
    __cpuid(0x00000002, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l2CacheSize = ((cpuInfo[2] >> 16) & 0xff) * 1024;
    printk("L2 Cache Size: %d\n", cpu_topology.l2CacheSize);

    // Get L3 Cache Size
    __cpuid(0x00000002, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l3CacheSize = ((cpuInfo[2] >> 8) & 0xff) * 1024 * 1024;
    printk("L3 Cache Size: %d\n", cpu_topology.l3CacheSize);

    // Get L4 Cache Size
    __cpuid(0x00000002, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l4CacheSize = ((cpuInfo[2] >> 0) & 0xff) * 1024 * 1024;
    printk("L4 Cache Size: %d\n", cpu_topology.l4CacheSize);

    // Get Current Frequency
    __cpuid(0x80000007, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.currentFrequency = cpuInfo[3];
    printk("Current Frequency: %d\n", cpu_topology.currentFrequency);

    // Get Max Frequency
    __cpuid(0x80000007, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.maxFrequency = cpuInfo[0];
    printk("Max Frequency: %d\n", cpu_topology.maxFrequency);

    // Get Min Frequency
    __cpuid(0x80000007, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.minFrequency = cpuInfo[1];
    printk("Min Frequency: %d\n", cpu_topology.minFrequency);

    pause();
}