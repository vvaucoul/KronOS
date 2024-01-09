/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topology.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 00:52:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 14:12:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cpu.h>

// TMP
#include <system/pit.h>

cpu_topology_t cpu_topology;

// static inline void cpuid(int code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
//     __asm__ volatile("cpuid":"=a"(*a),"=b"(*b),"=c"(*c),"=d"(*d):"a"(code));
// }

/* Check Flags HTT */
static bool cpuid_is_supported(void)
{
    uint32_t eax, ebx, ecx, edx;
    uint32_t htt = 0;

    __cpuid(0x00000001, eax, ebx, ecx, edx);
    htt = (edx >> 28) & 1;

    if (htt == 1)
        printk("\t\t "_YELLOW
               "[CID]"_END
               " - "_GREEN
               "%s" _END "\n",
               "[SUPPORTED]");
    else
        printk("\t\t "_YELLOW
               "[CID]"_END
               " - "_RED
               "%s" _END "\n",
               "[NOT SUPPORTED]");

    // uint32_t count = (ebx >> 23) & 0x1F;
    // uint32_t count_max = (ebx >> 16) & 0xFF;

    return (htt == 1);
}

void get_cpu_topology(void)
{
    // CPUID eax = 0x00000001;

    // Check if CPUID is supported (with flag HTT)
    if (cpuid_is_supported() == false)
        return;
    else
    {
        __cpuid_available = true;
        memset(&cpu_topology, 0, sizeof(cpu_topology));
    }

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

    // Get the number of physical cores per package.
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.physicalCoresPerPackage = (cpuInfo[0] >> 26) + 1;

    // Get the number of logical cores per physical core.
    __cpuid(0x00000001, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.logicalCoresPerPhysicalCore = ((cpuInfo[1] >> 16) & 0xff);

    // Get Socket count
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.socketCount = ((cpuInfo[0] >> 14) & 0xfff) + 1;

    // Get Core Count
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.coreCount = ((cpuInfo[0] >> 26) & 0x3f) + 1;

    // Get Thread Count
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.threadCount = ((cpuInfo[0] >> 14) & 0xfff) + 1;

    // Get L1 Cache Size
    __cpuid(0x00000005, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l1CacheSize = cpuInfo[2] * 1024;

    // Get L2 Cache Size
    __cpuid(0x00000006, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l2CacheSize = ((cpuInfo[2] >> 16) & 0xffff) * 1024;

    // Get L3 Cache Size
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    cpu_topology.l3CacheSize = (((cpuInfo[2] >> 22) + 1) * ((cpuInfo[2] & 0x3ff) + 1) * ((cpuInfo[2] >> 12) & 0x3ff) * (cpuInfo[1] & 0xff)) * 64;

    // Get Current Frequency
    cpu_topology.currentFrequency = get_cpu_frequency();

    // printk("CPU Topology:\n");
    // printk("\tPhysical cores per package: %d\n", cpu_topology.physicalCoresPerPackage);
    // printk("\tLogical cores per physical core: %d\n", cpu_topology.logicalCoresPerPhysicalCore);
    // printk("\tSocket count: %d\n", cpu_topology.socketCount);
    // printk("\tCore count: %d\n", cpu_topology.coreCount);
    // printk("\tThread count: %d\n", cpu_topology.threadCount);
    // printk("\tL1 Cache Size: %d\n", cpu_topology.l1CacheSize);
    // printk("\tL2 Cache Size: %d\n", cpu_topology.l2CacheSize);
    // printk("\tL3 Cache Size: %d\n", cpu_topology.l3CacheSize);
    // printk("\tCurrent Frequency: %d MHz\n", cpu_topology.currentFrequency);
    // printk("\tCPU Brand: %s\n", cpu_topology.brandString);

    // kpause();

    printk(_END "\t\t\t   -"_GREEN
                " VENDOR: " _END "%s" _END "\n",
           cpu_vendor);
    printk(_END "\t\t\t   -"_GREEN
                " HYPERVISOR: " _END "%s" _END "\n",
           hypervisor);
}