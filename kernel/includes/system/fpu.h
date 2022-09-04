/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fpu.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/04 16:53:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/04 19:16:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FPU_H
#define FPU_H

#define __FPU_INIT 0x37F
#define __FPU_INVALID_OPERAND 0x37E
#define __FPU_DIVIDE_BY_ZERO 0x37A

/* Enable Float Point Units */
void enable_fpu(void);

#endif /* FPU */