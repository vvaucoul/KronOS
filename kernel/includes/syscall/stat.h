/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 13:23:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 15:36:09 by vvaucoul         ###   ########.fr       */
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

#define S_IFMT 0170000   // mask to extract the file type
#define S_IFDIR 0040000  // directory
#define S_IFCHR 0020000  // character device
#define S_IFBLK 0060000  // block device
#define S_IFREG 0100000  // regular file
#define S_IFIFO 0010000  // FIFO
#define S_IFLNK 0120000  // symbolic link
#define S_IFSOCK 0140000 // socket

#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)   // check if it's a directory
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)   // check if it's a character device
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)   // check if it's a block device
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)   // check if it's a regular file
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)  // check if it's a FIFO
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)   // check if it's a symbolic link
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK) // check if it's a socket

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