/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sys_list.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 19:27:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 00:37:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYS_LIST_H
#define SYS_LIST_H

#include <kernel.h>
#include <multitasking/process.h>

typedef unsigned int mode_t;
struct stat;

extern int sys_open(const char *path, int flags, mode_t mode);
extern int sys_write(int fd, const void *buf, uint32_t count);
extern int sys_creat(const char *path, mode_t mode);
extern int sys_chdir(const char *path);
extern int sys_mkdir(const char *pathname, mode_t mode);

extern int sys_stat(const char *pathname, struct stat *statbuf);
extern int sys_fstat(int fd, struct stat *statbuf);
extern int sys_lstat(const char *pathname, struct stat *statbuf);

extern void exit(int32_t status);
extern pid_t fork(void);

extern char *getcwd(char *buf, uint32_t size);
extern char *getwd(char *buf);
extern char *get_current_dir_name(void);

extern int builtin_kill(int argc, char **argv);
extern int kill(pid_t pid, int32_t sig);

extern uid_t getuid(void);
extern uid_t geteuid(void);
extern void setuid(uid_t uid);
extern void seteuid(uid_t uid);

extern pid_t wait(int *status);
extern pid_t waitpid(pid_t pid, int *status, int options);

#endif /* !SYS_LIST_H */