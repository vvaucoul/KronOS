/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   linked_list.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/22 14:12:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/22 14:18:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LINKED_LIST_H
# define __LINKED_LIST_H

typedef struct __s_linked_list
{
    void *data;
    struct __s_linked_list *next;
} __attribute((packed, aligned(8))) __t_linked_list;

typedef __t_linked_list t_list;

#define List t_list

#endif /* !__LINKED_LIST_H */