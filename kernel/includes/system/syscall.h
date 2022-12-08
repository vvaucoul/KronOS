/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/08 12:21:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSCALL_H
# define SYSCALL_H

#include <kernel.h>

/*
+-------+------------+
| %rax  |    Name    |
+-------+------------+
| 0     | read       |
| 1     | write      |
| 2     | open       |
| 3     | close      |
| 4     | stat       |
| 5     | fstat      |
| 6     | lstat      |
| 7     | poll       |
| 8     | lseek      |
| 9     | mmap       |
| 10    | mprotect   |
| 11    | munmap     |
| 12    | brk        |
| 13    | rt_sigaction| 
| 14    | rt_sigprocmask|
| 15    | rt_sigreturn| stub_rt_sigreturn| arch/x86/kernel/signal.c |
| 16    | ioctl  | sys_ioctl  | fs/ioctl.c      |
| 17    | pread64| sys_pread64| fs/read_write.c |
| 18    | pwrite64| sys_pwrite64| fs/read_write.c |
| 19    | readv  | sys_readv  | fs/read_write.c |
| 20    | writev | sys_writev | fs/read_write.c |
| 21    | access | sys_access | fs/open.c       |
| 22    | pipe   | sys_pipe   | fs/pipe.c       |
| 23    | select | sys_select | fs/select.c     |
| 24    | sched_yield| sys_sched_yield| kernel/sched/core.c |
| 25    | mremap | sys_mremap | mm/mmap.c       |
| 26    | msync  | sys_msync  | mm/msync.c      |
| 27    | mincore| sys_mincore| mm/mincore.c    |
| 28    | madvise| sys_madvise| mm/madvise.c    |
| 29    | shmget | sys_shmget | ipc/shm.c       |
| 30    | shmat  | sys_shmat  | ipc/shm.c       |
| 31    | shmctl | sys_shmctl | ipc/shm.        |
| 32    | dup    | sys_dup    | fs/file_table.c |
| 33    | dup2   | sys_dup2   | fs/file_table.c |
| 34    | pause  | sys_pause  | kernel/signal.c |
| 35    | nanosleep| sys_nanosleep| kernel/time/timekeeping.c |
| 36    | getitimer| sys_getitimer| kernel/time/timekeeping.c |
| 37    | alarm  | sys_alarm  | kernel/time/timekeeping.c |


*/

#endif /* !SYSCALL_H */