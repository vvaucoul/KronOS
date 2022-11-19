/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ordered_array.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 09:57:50 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/19 10:02:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ORDERED_ARRAY_H
# define ORDERED_ARRAY_H

# include <kernel.h>

typedef void *type_t;

typedef int8_t (*lessthan_predicate_t)(type_t, type_t);

typedef struct s_ordered_array
{
    type_t *array;
    uint32_t size;
    uint32_t max_size;
    lessthan_predicate_t less_than;
} ordered_array_t;

extern int8_t standard_lessthan_predicate(type_t a, type_t b);
extern ordered_array_t create_ordered_array(uint32_t max_size, lessthan_predicate_t less_than);
extern ordered_array_t place_ordered_array(void *addr, uint32_t max_size, lessthan_predicate_t less_than);
extern void destroy_ordered_array(ordered_array_t *array);
extern void insert_ordered_array(type_t item, ordered_array_t *array);
extern type_t lookup_ordered_array(uint32_t i, ordered_array_t *array);
extern void remove_ordered_array(uint32_t i, ordered_array_t *array);

#endif /* ORDERED_ARRAY_H */