/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tinyfs_fops.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:49:34 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/09 13:29:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/tinyfs/tinyfs.h>

/**
 * @brief Read data from file
 *
 * @param node
 * @param offset
 * @param size
 * @param buffer
 *
 * @note This function will read data from file
 *
 * @return int
 */
int tinyfs_read(__unused__ void *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    return (tinyfs_device->read(tinyfs_device->device, offset, size, buffer));
}

/**
 * @brief Write data to file
 *
 * @param node
 * @param offset
 * @param size
 * @param buffer
 *
 * @note This function will write data to file
 *
 * @return int
 */
int tinyfs_write(__unused__ void *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    return (tinyfs_device->write(tinyfs_device->device, offset, size, buffer));
}