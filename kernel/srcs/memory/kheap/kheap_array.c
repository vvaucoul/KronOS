/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kheap_array.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 16:47:00 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/06 12:10:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/kheap.h>

bool heap_predicate(data_t a, data_t b)
{
    return (a < b) ? true : false;
}

heap_array_t heap_array_create(void *addr, uint32_t max_size, heap_node_predicate_t predicate)
{
    heap_array_t heap_array;

    heap_array.array = (data_t *)addr;
    memset(heap_array.array, 0, max_size * sizeof(data_t));
    heap_array.size = 0;
    heap_array.max_size = max_size;
    heap_array.predicate = predicate;
    return (heap_array);
}

void heap_array_insert_element(data_t data, heap_array_t *array)
{
    assert(array->predicate != NULL);

    uint32_t index = 0;

    while (index < array->size && array->predicate(array->array[index], data))
        index++;

    if (index == array->size)
    {
        array->array[array->size++] = data;
    }
    else
    {
        data_t tmp = array->array[index];
        array->array[index] = data;

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
    assert(index <= array->size);
    return (array->array[index]);
}

void heap_array_remove_element(uint32_t index, heap_array_t *array)
{
    assert(index <= array->size);

    while (index < array->size)
    {
        array->array[index] = array->array[index + 1];
        index++;
    }
    array->size--;
}

void heap_destroy(heap_array_t *array)
{
    kfree(array->array);
}