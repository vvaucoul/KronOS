/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fpu.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 16:53:20 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/02/11 20:23:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/fpu.h>
#include <kernel.h>

static void fpu_set_control_word(const uint16_t __control_word)
{
    __asm__ volatile("fldcw %0" :: "m"((__control_word)));
}

void enable_fpu(void)
{
    uint32_t cr4;
    __asm__ volatile("mov %%cr4, %0" :"=r"(cr4));
    cr4 |= 0X200;
    __asm__ volatile("mov %0, %%cr4" :: "r"((cr4)));
    fpu_set_control_word(__FPU_INIT);
    fpu_set_control_word(__FPU_INVALID_OPERAND);
    fpu_set_control_word(__FPU_DIVIDE_BY_ZERO);
}