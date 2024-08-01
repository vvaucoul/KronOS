/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ealloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 18:07:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/31 19:04:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EALLOC_H
#define EALLOC_H

#include <stdint.h>

void *ealloc(uint32_t size);
void *ealloc_aligned(uint32_t size);
void *ealloc_aligned_physic(uint32_t size, uint32_t *phys);

void *ecalloc(uint32_t size);
void einsert(void *ptr, uint32_t addr, uint32_t size);

uint32_t get_placement_addr(void);
void set_placement_addr(uint32_t addr);

#endif /* !EALLOC_H */
