/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irq.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 19:54:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/07/11 12:28:46 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRQ_H
#define IRQ_H

#include <kernel.h>
#include <system/idt.h>
#include <system/isr.h>
#include <system/io.h>

void irq_install();
void irq_handler(struct regs *r);
void irq_install_handler(int irq, void (*handler)(struct regs *r));

#endif // !IRQ_H