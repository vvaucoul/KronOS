/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 09:43:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 21:27:35 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INITRD_H
#define INITRD_H

#include <filesystem/ext2/ext2.h>

/**
* @file initrd.h
* @brief Initrd Filesystem
*
* This file contains the definition and structures related to the Initrd filesystem.
*
* The Initrd filesystem is a temporary filesystem used during the early stages of the kernel boot process.
* It provides support for file operations such as reading, writing, opening, closing, and directory listing.
*
* The Initrd filesystem implementation includes structures such as InitrdHeader and InitrdFileHeader,
* which represent the header and file information of the initrd image, respectively.
*
* It also defines functions for initializing the initrd filesystem, debugging, and displaying the hierarchy.
* The Initrd filesystem module is part of the kernel's filesystem infrastructure and is used to load
* essential files and drivers required for the kernel to boot successfully. 
*/

#define FS_OPEN 0x01

typedef struct {
    uint32_t nfiles;
} InitrdHeader;

typedef struct {
    uint8_t magic;
    int8_t name[64];
    uint32_t offset;
    uint32_t length;
} InitrdFileHeader;

extern Ext2Inode *initrd_init(uint32_t location);

// Debug
extern void initrd_display_hierarchy(void);
#endif /* INITRD_H */