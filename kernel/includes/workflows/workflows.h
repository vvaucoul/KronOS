/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflows.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/28 13:38:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/30 15:24:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __WORKFLOWS_H
# define __WORKFLOWS_H

#include <kernel.h>

/* Kernel Memory Map */
extern void display_kernel_memory_map(void);

/* Multiboot Info */
extern void display_multiboot_infos(void);

/* Physical memory manager */
extern void pmm_display(void);
extern int pmm_test(void);
extern int pmm_defragment_test(void);

/* Kernel Heap */
extern void kheap_test(void);

#endif /* !__WORKFLOWS_H */