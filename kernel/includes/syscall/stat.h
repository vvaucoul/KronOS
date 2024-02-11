/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 13:23:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 13:30:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STAT_H
#define STAT_H

/**
 * This code snippet is related to the stat.h file.
 * It contains functions and structures for working with file status.
 *
 * The stat structure is used to store information about a file.
 * The stat function is used to get information about a file.
 */

struct stat {
    uint16_t st_dev;     /* ID of the device containing the file */
    uint32_t st_ino;     /* Inode number */
    uint32_t st_mode;    /* Protection */
    uint32_t st_nlink;   /* Number of hard links */
    uint8_t st_uid;      /* Owner's user ID */
    uint8_t st_gid;      /* Owner's group ID */
    uint8_t st_rdev;     /* ID of the device (if special file) */
    uint32_t st_size;    /* Total size in bytes */
    uint32_t st_blksize; /* Block size for file system I/O */
    uint16_t st_blocks;  /* Number of allocated blocks */
    uint64_t st_atime;   /* Time of last access */
    uint64_t st_mtime;   /* Time of last modification */
    uint64_t st_ctime;   /* Time of last status change */
};

extern int stat(const char *path, struct stat *buf);

#endif /* !STAT_H */