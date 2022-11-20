/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap_array.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 16:47:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/11/20 13:28:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

heap_array_t heap_array_create(void *addr, uint32_t max_size, heap_node_predicate_t predicate)
{
    heap_array_t heap_array;

    heap_array.array = (data_t *)addr;
    memset(heap_array.array, 0, max_size * sizeof(data_t));
    heap_array.max_size = max_size;
    heap_array.size = 0;
    heap_array.predicate = predicate;

    return (heap_array);
}

void heap_array_insert_element(data_t data, heap_array_t *array)
{
    assert(array->predicate != NULL);

    uint32_t index = 0;

    /* Find the right place to insert the data */
    while (index < array->size && array->predicate(array->array[index], data) == 1)
        index++;

    /* If the index is at the end of the array, just add the data at the end */
    if (index == array->size)
    {
        array->array[array->size] = data;
        array->size++;
    }
    /* Else, we need to shift the array to the right */
    else
    {
        data_t tmp = array->array[index];
        array->array[index] = data;

        /* Move all the elements after the index */
        while (index < array->size)
        {
            index++;
            data_t tmp2 = array->array[index];
            array->array[index] = tmp;
            tmp = tmp2;
        }
        array->size++;
    }
}

data_t heap_array_get_element(uint32_t index, heap_array_t *array)
{
    assert(index < array->size);
    return (array->array[index]);
}

void heap_array_remove_element(uint32_t index, heap_array_t *array)
{
    assert(index < array->size);

    /* Shift all the elements after the index to the left */
    while (index < array->size)
    {
        array->array[index] = array->array[index + 1];
        index++;
    }
    array->size--;
}