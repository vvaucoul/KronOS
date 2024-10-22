/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pagetable_pool.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 13:35:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/22 15:57:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PAGETABLE_POOL_H
#define PAGETABLE_POOL_H

#include <stddef.h>
#include <stdint.h>

#define PAGE_TABLE_POOL_SIZE 0x10000 // 64 KB pool for page tables

extern void pagetable_pool_init(void);
extern page_table_t *pagetable_pool_alloc(void);

#endif // PAGETABLE_POOL_H