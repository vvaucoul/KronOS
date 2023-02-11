/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topology.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 00:52:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 13:48:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Detect cpu topology (Needed for Scheduler (multitasking))
// -> https://wiki.osdev.org/Detecting_CPU_Topology_(80x86)

#include <system/cpu.h>

cpu_topology_t cpu_topology;

void get_cpu_topology(void)
{
    int cpuInfo[4] = {0, 0, 0, 0};
    unsigned nExIds, i = 0;

    // Get the information associated with each extended ID.
    __cpuid(0x80000000, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    nExIds = cpuInfo[0];

    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(i, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);

        // Interpret CPU brand string and cache information.
        if  (i == 0x80000002)
            memcpy(cpu_topology.brandString, cpuInfo, sizeof(cpuInfo));
        else if  (i == 0x80000003)
            memcpy(cpu_topology.brandString + 16, cpuInfo, sizeof(cpuInfo));
        else if  (i == 0x80000004)
            memcpy(cpu_topology.brandString + 32, cpuInfo, sizeof(cpuInfo));
    }

    // Display all information in a human readable format.
    printk("CPU Brand: %s\n", cpu_topology.brandString);

    // Get the number of logical cores per physical core.
    __cpuid(0x00000001, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    printk("Logical cores per physical core: %d\n", ((cpuInfo[1] >> 16) & 0xff));

    // Get the number of threads per physical core.
    __cpuid(0x00000004, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    printk("Threads per physical core: %d\n", (cpuInfo[0] & 0x1f));

    // Get the number of cores per package.
    printk("Cores per package: %d\n", ((cpuInfo[0] >> 26) & 0x3f));

    // Get the number of packages.
    __cpuid(0x0000000b, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    printk("Packages: %d\n", (cpuInfo[1] & 0xffff));

    // Get the number of logical cores.
    __cpuid(0x0000000b, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    printk("Logical cores: %d\n", (cpuInfo[1] >> 16));

    

}