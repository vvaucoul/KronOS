/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 09:43:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/13 19:24:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INITRD_H
#define INITRD_H

#include <kernel.h>
#include <filesystem/vfs.h>

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

#define INITRD_FILE_SIZE 64
#define INITD_MAGIC 0xBF
#define INITRD_FILESYSTEM_NAME "initrd"
#define INITRD_DEV_NAME "dev"

typedef struct {
    uint32_t nfiles;
} __attribute__((packed)) InitrdHeader;

typedef struct {
    uint8_t magic;
    int8_t name[INITRD_FILE_SIZE];
    uint32_t offset;
    uint32_t size;
} __attribute__((packed)) InitrdFileHeader;

extern Vfs *initrd_fs;

extern int initrd_init(uint32_t start, uint32_t end);

#endif /* INITRD_H */