/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   isr.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:16:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/11 12:28:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ISR_H
#define ISR_H

#include <kernel.h>
#include <system/idt.h>

#define ISR_MAX_COUNT 32

struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

void isrs_install();

#endif // !ISR_H