/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ext2_formater.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 22:42:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/08 22:43:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <filesystem/ext2/ext2_formater.h>


// Todo: Formater
// static int ext2_create_superblock(Ext2SuperBlock *sb) {
//     if (sb == NULL) {
//         return (1);
//     }

//     memset(sb, 0, sizeof(Ext2SuperBlock));
//     sb->signature = EXT2_MAGIC;

//     if (ide_get_device(0) == NULL) {
//         printk("Failed to get device\n");
//         return (1);
//     }

//     ide_write(ide_devices[0], 2, 2, sb);

//     return (0);
// }