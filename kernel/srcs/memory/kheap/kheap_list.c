// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   kheap_list.c                                       :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/10/20 02:00:00 by vvaucoul          #+#    #+#             */
// /*   Updated: 2024/10/20 01:20:57 by vvaucoul         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include <mm/ealloc.h>
// #include <mm/mm.h>

// #include <assert.h> // for assert
// #include <stdio.h>	// for printk or qemu_printf
// #include <string.h> // for memset

// #include <system/serial.h> // qemu_printf

// /* Create a new heap list */
// heap_list_t heap_list_create(void *addr, uint32_t max_size, heap_node_predicate_t predicate) {
// 	heap_list_t list;
// 	list.head = NULL;
// 	list.tail = NULL;
// 	list.predicate = predicate;
// 	qemu_printf("Creating heap list with max size %u\n", max_size);
// 	return list;
// }

// /* Create a new heap node */
// static heap_node_t *heap_node_create(heap_header_t *header) {
// 	heap_node_t *node = (heap_node_t *)kmalloc(sizeof(heap_node_t));
// 	if (!node) {
// 		qemu_printf("Error: Unable to allocate memory for heap node\n");
// 		return NULL;
// 	}
// 	node->header = header;
// 	node->prev = NULL;
// 	node->next = NULL;
// 	return node;
// }

// /* Insert a heap_header_t* into the heap list based on the predicate */
// void heap_list_insert_element(heap_header_t *header, heap_list_t *list) {
// 	assert(header != NULL);
// 	assert(list != NULL);

// 	heap_node_t *new_node = heap_node_create(header);
// 	if (!new_node) {
// 		// Handle allocation failure
// 		qemu_printf("Error: Failed to create new heap node\n");
// 		return;
// 	}

// 	qemu_printf("Inserting heap block at %p into heap list\n", header);

// 	// If list is empty
// 	if (list->head == NULL) {
// 		list->head = new_node;
// 		list->tail = new_node;
// 		return;
// 	}

// 	// Find the correct position based on predicate
// 	heap_node_t *current = list->head;
// 	while (current != NULL && list->predicate(current->header, header)) {
// 		current = current->next;
// 	}

// 	if (current == NULL) {
// 		// Insert at the end
// 		list->tail->next = new_node;
// 		new_node->prev = list->tail;
// 		list->tail = new_node;
// 	} else if (current == list->head) {
// 		// Insert at the beginning
// 		new_node->next = list->head;
// 		list->head->prev = new_node;
// 		list->head = new_node;
// 	} else {
// 		// Insert before current
// 		new_node->next = current;
// 		new_node->prev = current->prev;
// 		current->prev->next = new_node;
// 		current->prev = new_node;
// 	}
// }

// /* Remove a heap_header_t* from the heap list */
// void heap_list_remove_element(heap_header_t *header, heap_list_t *list) {
// 	assert(header != NULL);
// 	assert(list != NULL);

// 	qemu_printf("Removing heap block at %p from heap list\n", header);

// 	heap_node_t *current = list->head;
// 	while (current != NULL) {
// 		if (current->header == header) {
// 			if (current->prev)
// 				current->prev->next = current->next;
// 			else
// 				list->head = current->next;

// 			if (current->next)
// 				current->next->prev = current->prev;
// 			else
// 				list->tail = current->prev;

// 			kfree(current);
// 			return;
// 		}
// 		current = current->next;
// 	}

// 	qemu_printf("Warning: Attempted to remove non-existent heap block %p\n", header);
// }

// /* Destroy the heap list and free all nodes */
// void heap_destroy_list(heap_list_t *list) {
// 	assert(list != NULL);
// 	qemu_printf("Destroying heap list\n");

// 	heap_node_t *current = list->head;
// 	while (current != NULL) {
// 		heap_node_t *next = current->next;
// 		kfree(current);
// 		current = next;
// 	}
// 	list->head = NULL;
// 	list->tail = NULL;
// }
