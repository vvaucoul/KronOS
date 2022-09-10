/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pmm.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/10 13:17:02 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/09/10 13:45:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMM_H
# define PMM_H

#include <kernel.h>

/*
** PMM: Physical Memory Manager
*/

#define PMM_BLOCK_SIZE 4096

typedef struct s_pmm
{
    uint32_t    size;
} t_pmm;

#define PMM_INFO t_pmm

#endif /* !PMM_H */