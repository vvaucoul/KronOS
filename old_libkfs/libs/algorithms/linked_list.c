/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   linked_list.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 14:17:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/22 14:21:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "linked_list.h"
#include "../stddef/stddef.h"

List *lst_new(void *data, uint32_t addr)
{
    List *new;

    new = (List *)addr;
    new->data = data;
    new->next = NULL;
    return (new);
}