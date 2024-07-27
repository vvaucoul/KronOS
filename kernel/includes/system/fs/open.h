/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/23 00:23:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 08:46:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPEN_H
#define OPEN_H

#include <multitasking/process.h>
#include <system/fs/stat.h> // Include stat.h for permission constants

#ifndef _OFF_T
typedef long off_t;
#define _OFF_T
#endif

// Constants for the flags parameter of the open() function
#define O_RDONLY 00         // Open for reading only
#define O_WRONLY 01         // Open for writing only
#define O_RDWR 02           // Open for reading and writing
#define O_CREAT 0100        // Create file if it does not exist
#define O_EXCL 0200         // Exclusive use flag
#define O_TRUNC 01000       // Truncate flag
#define O_APPEND 02000      // Append flag
#define O_NONBLOCK 04000    // Non-blocking flag
#define O_NDELAY O_NONBLOCK // Same as O_NONBLOCK
#define O_SYNC 04010000     // Synchronous write flag
#define O_ASYNC 020000      // Asynchronous flag

// Other constants
#define F_DUPFD 0  // Duplicate file descriptor
#define F_GETFD 1  // Get file descriptor flags
#define F_SETFD 2  // Set file descriptor flags
#define F_GETFL 3  // Get file status flags
#define F_SETFL 4  // Set file status flags
#define F_GETLK 5  // Get record locking information
#define F_SETLK 6  // Set record locking information
#define F_SETLKW 7 // Set record locking information; wait if blocked

// Structure for file locks
struct flock {
    short l_type;   // Type of lock: F_RDLCK, F_WRLCK, F_UNLCK
    short l_whence; // How to interpret l_start: SEEK_SET, SEEK_CUR, SEEK_END
    off_t l_start;  // Starting offset for lock
    off_t l_len;    // Number of bytes to lock
    pid_t l_pid;    // PID of process blocking our lock (F_GETLK only)
};

#endif /* !OPEN_H */