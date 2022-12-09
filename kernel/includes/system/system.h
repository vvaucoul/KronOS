/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/22 21:22:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/09 22:36:05 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <system/gdt.h>
#include <system/idt.h>
#include <system/irq.h>
#include <system/isr.h>

#include <system/io.h>
#include <system/pit.h>

#include <system/panic.h>
#include <system/kernno.h>

#endif /* !__SYSTEM_H */