/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perm.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 23:53:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 10:47:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERM_H
#define PERM_H

#include <kernel.h>
#include <fs/vfs/vfs.h>

extern int check_permissions(uint32_t perm, uint32_t mode);
extern int check_inode_permissions(VfsNode *inode, uint32_t mode);

#endif /* !PERM_H */