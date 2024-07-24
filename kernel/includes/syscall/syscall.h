/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 22:30:56 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/24 19:35:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <kernel.h>
#include <syscall/sys_list.h>

/**
 * @file syscall.h
 * @brief Header file containing the declarations and macros for system calls.
 *
 * This file defines the system call structure, function prototypes, and macros for making system calls.
 * It also includes other header files related to specific system calls.
 */

/*
All x86 syscall: https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md#x86-32_bit
*/

#define SYSCALL_SIZE 0x180 // 384 syscall
#define SYSCALL_IRQ 0x80

// typedef void sysfn_t;
typedef int32_t (*sysfn_t)(void *, void *, void *);

typedef unsigned int mode_t;

typedef struct s_syscall {
    uint32_t id;
    char *name;
    sysfn_t function;
} syscall_t;

extern void init_syscall(void);
extern void isr80_handler(struct regs *r);

// ! ||--------------------------------------------------------------------------------||
// ! ||                                 SYSCALL MACROS                                 ||
// ! ||--------------------------------------------------------------------------------||

#define _syscall0(type, name)                 \
    type name(void) {                         \
        type __res;                           \
        __asm__ volatile("int $0x80"          \
                         : "=a"(__res)        \
                         : "0"(__NR_##name)); \
        if (__res >= 0)                       \
            return __res;                     \
        return -1;                            \
    }

#define _syscall(type, name, atype, a)                \
    type name(atype a) {                              \
        type __res;                                   \
        __asm__ volatile("int $0x80"                  \
                         : "=a"(__res)                \
                         : "0"(__NR_##name), "b"(a)); \
        if (__res >= 0)                               \
            return __res;                             \
        return -1;                                    \
    }

#define _syscall1(type, name, atype, a, btype, b)             \
    type name(atype a, btype b) {                             \
        type __res;                                           \
        __asm__ volatile("int $0x80"                          \
                         : "=a"(__res)                        \
                         : "0"(__NR_##name), "b"(a), "c"(b)); \
        if (__res >= 0)                                       \
            return __res;                                     \
        return -1;                                            \
    }

#define _syscall2(type, name, atype, a, btype, b, ctype, c)           \
    type name(atype a, btype b, ctype c) {                            \
        type __res;                                                   \
        __asm__ volatile("int $0x80"                                  \
                         : "=a"(__res)                                \
                         : "0"(__NR_##name), "b"(a), "c"(b), "d"(c)); \
        if (__res >= 0)                                               \
            return __res;                                             \
        return -1;                                                    \
    }

#define _syscall3(type, name, atype, a, btype, b, ctype, c, dtype, d)         \
    type name(atype a, btype b, ctype c, dtype d) {                           \
        type __res;                                                           \
        __asm__ volatile("int $0x80"                                          \
                         : "=a"(__res)                                        \
                         : "0"(__NR_##name), "b"(a), "c"(b), "d"(c), "S"(d)); \
        if (__res >= 0)                                                       \
            return __res;                                                     \
        return -1;                                                            \
    }

#define _syscall4(type, name, atype, a, btype, b, ctype, c, dtype, d, etype, e)       \
    type name(atype a, btype b, ctype c, dtype d, etype e) {                          \
        type __res;                                                                   \
        __asm__ volatile("int $0x80"                                                  \
                         : "=a"(__res)                                                \
                         : "0"(__NR_##name), "b"(a), "c"(b), "d"(c), "S"(d), "D"(e)); \
        if (__res >= 0)                                                               \
            return __res;                                                             \
        return -1;                                                                    \
    }

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  SYSCALL LIST                                  ||
// ! ||--------------------------------------------------------------------------------||

/**
** %rax  System call          %rdi                         %rsi                          %rdx                          %r10                           %r8                           %r9
** ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
** 0     sys_read             unsigned int fd              char *buf                     size_t count
** 1     sys_write            unsigned int fd              const char *buf               size_t count
** 2     sys_open             const char *filename         int flags                     int mode
** 3     sys_close            unsigned int fd
** 4     sys_stat             const char *filename         struct stat *statbuf
** 5     sys_fstat            unsigned int fd              struct stat *statbuf
** 6     sys_lstat            const char *filename         struct stat *statbuf
** 7     sys_poll             struct poll_fd *ufds         unsigned int nfds             long timeout_msecs
** 8     sys_lseek            unsigned int fd              off_t offset                  unsigned int origin
** 9     sys_mmap             unsigned long addr           unsigned long len             unsigned long prot               unsigned long flags               unsigned long fd                 unsigned long off
** 10    sys_mprotect         unsigned long start          size_t len                    unsigned long prot
** 11    sys_munmap           unsigned long addr           size_t len
** 12    sys_brk              unsigned long brk
** 13    sys_rt_sigaction     int sig                      const struct sigaction *act   struct sigaction *oact           size_t sigsetsize
** 14    sys_rt_sigprocmask   int how                      sigset_t *nset                sigset_t *oset                   size_t sigsetsize
** 15    sys_rt_sigreturn     unsigned long __unused
** 16    sys_ioctl            unsigned int fd              unsigned int cmd              unsigned long arg
** 17    sys_pread64          unsigned long fd             char *buf                     size_t count                     loff_t pos
** 18    sys_pwrite64         unsigned int fd              const char *buf               size_t count                     loff_t pos
** 19    sys_readv            unsigned long fd             const struct iovec *vec       unsigned long vlen
** 20    sys_writev           unsigned long fd             const struct iovec *vec       unsigned long vlen
** 21    sys_access           const char *filename         int mode
** 22    sys_pipe             int *filedes
** 23    sys_select           int n                        fd_set *inp                   fd_set *outp                     fd_set *exp                     struct timeval *tvp
** 24    sys_sched_yield
** 25    sys_mremap           unsigned long addr           unsigned long old_len         unsigned long new_len            unsigned long flags               unsigned long new_addr
** 26    sys_msync            unsigned long start          size_t len                    int flags
** 27    sys_mincore          unsigned long start          size_t len                    unsigned char *vec
** 28    sys_madvise          unsigned long start          size_t len_in                 int behavior
** 29    sys_shmget           key_t key                    size_t size                   int shmflg
** 30    sys_shmat            int shmid                    char *shmaddr                 int shmflg
** 31    sys_shmctl           int shmid                    int cmd                       struct shmid_ds *buf
** 32    sys_dup              unsigned int fildes
** 33    sys_dup2             unsigned int oldfd           unsigned int newfd
** 34    sys_pause
** 35    sys_nanosleep        struct timespec *rqtp        struct timespec *rmtp
** 36    sys_getitimer        int which                    struct itimerval *value
** 37    sys_alarm            unsigned int seconds
** 38    sys_setitimer        int which                    struct itimerval *value       struct itimerval *ovalue
** 39    sys_getpid
** 40    sys_sendfile         int out_fd                   int in_fd                     off_t *offset                    size_t count
** 41    sys_socket           int family                   int type                      int protocol
** 42    sys_connect          int fd                       struct sockaddr *uservaddr    int addrlen
** 43    sys_accept           int fd                       struct sockaddr *upeer_sockaddr int *upeer_addrlen
** 44    sys_sendto           int fd                       void *buff                    size_t len                       unsigned flags                   struct sockaddr *addr             int addr_len
** 45    sys_recvfrom         int fd                       void *ubuf                    size_t size                      unsigned flags                   struct sockaddr *addr             int *addr_len
** 46    sys_sendmsg          int fd                       struct msghdr *msg            unsigned flags
** 47    sys_recvmsg          int fd                       struct msghdr *msg            unsigned int flags
** 48    sys_shutdown         int fd                       int how
** 49    sys_bind             int fd                       struct sockaddr *umyaddr      int addrlen
** 50    sys_listen           int fd                       int backlog
** 51    sys_getsockname      int fd                       struct sockaddr *usockaddr    int *usockaddr_len
** 52    sys_getpeername      int fd                       struct sockaddr *usockaddr    int *usockaddr_len
** 53    sys_socketpair       int family                   int type                      int protocol                     int *usockvec
** 54    sys_setsockopt       int fd                       int level                     int optname                      char *optval                     int optlen
** 55    sys_getsockopt       int fd                       int level                     int optname                      char *optval                     int *optlen
** 56    sys_clone            unsigned long clone_flags    unsigned long newsp           void *parent_tid                 void *child_tid                 unsigned int tid
** 57    sys_fork
** 58    sys_vfork
** 59    sys_execve           const char *filename         const char *const argv[]      const char *const envp[]
** 60    sys_exit             int error_code
** 61    sys_wait4            pid_t upid                   int *stat_addr                int options                      struct rusage *ru
** 62    sys_kill             pid_t pid                    int sig
** 63    sys_uname            struct old_utsname *name
** 64    sys_semget           key_t key                    int nsems                     int semflg
** 65    sys_semop            int semid                    struct sembuf *tsops          unsigned nsops
** 66    sys_semctl           int semid                    int semnum                    int cmd                          union semun arg
** 67    sys_shmdt            char *shmaddr
** 68    sys_msgget           key_t key                    int msgflg
** 69    sys_msgsnd           int msqid                    struct msgbuf *msgp           size_t msgsz                     int msgflg
** 70    sys_msgrcv           int msqid                    struct msgbuf *msgp           size_t msgsz                     long msgtyp                     int msgflg
** 71    sys_msgctl           int msqid                    int cmd                       struct msqid_ds *buf
** 72    sys_fcntl            unsigned int fd              unsigned int cmd              unsigned long arg
** 73    sys_flock            unsigned int fd              unsigned int cmd
** 74    sys_fsync            unsigned int fd
** 75    sys_fdatasync        unsigned int fd
** 76    sys_truncate         const char *path             long length
** 77    sys_ftruncate        unsigned int fd              unsigned long length
** 78    sys_getdents         unsigned int fd              struct linux_dirent *dirent   unsigned int count
** 79    sys_getcwd           char *buf                    unsigned long size
** 80    sys_chdir            const char *filename
** 81    sys_fchdir           unsigned int fd
** 82    sys_rename           const char *oldname          const char *newname
** 83    sys_mkdir            const char *pathname         int mode
** 84    sys_rmdir            const char *pathname
** 85    sys_creat            const char *pathname         int mode
** 86    sys_link             const char *oldname          const char *newname
** 87    sys_unlink           const char *pathname
** 88    sys_symlink          const char *oldname          const char *newname
** 89    sys_readlink         const char *path             char *buf                     int bufsiz
** 90    sys_chmod            const char *filename         mode_t mode
** 91    sys_fchmod           unsigned int fd              mode_t mode
** 92    sys_chown            const char *filename         uid_t user                    gid_t group
** 93    sys_fchown           unsigned int fd              uid_t user                    gid_t group
** 94    sys_lchown           const char *filename         uid_t user                    gid_t group
** 95    sys_umask            int mask
** 96    sys_gettimeofday     struct timeval *tv           struct timezone *tz
** 97    sys_getrlimit        unsigned int resource        struct rlimit *rlim
** 98    sys_getrusage        int who                      struct rusage *ru
** 99    sys_sysinfo          struct sysinfo *info
** 100   sys_times            struct tms *tbuf
** 101   sys_ptrace           long request                 long pid                      unsigned long addr               unsigned long data
** 102   sys_getuid
** 103   sys_syslog           int type                     char *buf                     int len
** 104   sys_getgid
** 105   sys_setuid           uid_t uid
** 106   sys_setgid           gid_t gid
** 107   sys_geteuid
** 108   sys_getegid
** 109   sys_setpgid          pid_t pid                    pid_t pgid
** 110   sys_getppid
** 111   sys_getpgrp
** 112   sys_setsid
** 113   sys_setreuid         uid_t ruid                   uid_t euid
** 114   sys_setregid         gid_t rgid                   gid_t egid
** 115   sys_getgroups        int gidsetsize               gid_t *grouplist
** 116   sys_setgroups        int gidsetsize               gid_t *grouplist
** 117   sys_setresuid        uid_t *ruid                  uid_t *euid                   uid_t *suid
** 118   sys_getresuid        uid_t *ruid                  uid_t *euid                   uid_t *suid
** 119   sys_setresgid        gid_t rgid                   gid_t egid                    gid_t sgid
** 120   sys_getresgid        gid_t *rgid                  gid_t *egid                   gid_t *sgid
** 121   sys_getpgid          pid_t pid
** 122   sys_setfsuid         uid_t uid
** 123   sys_setfsgid         gid_t gid
** 124   sys_getsid           pid_t pid
** 125   sys_capget           cap_user_header_t header     cap_user_data_t dataptr
** 126   sys_capset           cap_user_header_t header     const cap_user_data_t data
** 127   sys_rt_sigpending    sigset_t *set                size_t sigsetsize
** 128   sys_rt_sigtimedwait  const sigset_t *uthese       siginfo_t *uinfo              const struct timespec *uts         size_t sigsetsize
** 129   sys_rt_sigqueueinfo  pid_t pid                    int sig                       siginfo_t *uinfo
** 130   sys_rt_sigsuspend    sigset_t *unewset            size_t sigsetsize
** 131   sys_sigaltstack      const stack_t *uss           stack_t *uoss
** 132   sys_utime            char *filename               struct utimbuf *times
** 133   sys_mknod            const char *filename         umode_t mode                  unsigned dev
** 134   sys_uselib           NOT IMPLEMENTED
** 135   sys_personality      unsigned int personality
** 136   sys_ustat            unsigned dev                 struct ustat *ubuf
** 137   sys_statfs           const char *pathname         struct statfs *buf
** 138   sys_fstatfs          unsigned int fd              struct statfs *buf
** 139   sys_sysfs            int option                   unsigned long arg1            unsigned long arg2
** 140   sys_getpriority      int which                    int who
** 141   sys_setpriority      int which                    int who                       int niceval
** 142   sys_sched_setparam   pid_t pid                    struct sched_param *param
** 143   sys_sched_getparam   pid_t pid                    struct sched_param *param
** 144   sys_sched_setscheduler pid_t pid                  int policy                    struct sched_param *param
** 145   sys_sched_getscheduler pid_t pid
** 146   sys_sched_get_priority_max int policy
** 147   sys_sched_get_priority_min int policy
** 148   sys_sched_rr_get_interval pid_t pid               struct timespec *interval
** 149   sys_mlock            unsigned long start          size_t len
** 150   sys_munlock          unsigned long start          size_t len
** 151   sys_mlockall         int flags
** 152   sys_munlockall
** 153   sys_vhangup
** 154   sys_modify_ldt       int func                     void *ptr                     unsigned long bytecount
** 155   sys_pivot_root       const char *new_root         const char *put_old
** 156   sys__sysctl          struct __sysctl_args *args
** 157   sys_prctl            int option                   unsigned long arg2            unsigned long arg3                unsigned long arg4              unsigned long arg5
** 158   sys_arch_prctl       struct task_struct *task     int code                      unsigned long *addr
** 159   sys_adjtimex         struct timex *txc_p
** 160   sys_setrlimit        unsigned int resource        struct rlimit *rlim
** 161   sys_chroot           const char *filename
** 162   sys_sync
** 163   sys_acct             const char *name
** 164   sys_settimeofday     struct timeval *tv           struct timezone *tz
** 165   sys_mount            char *dev_name               char *dir_name                char *type                       unsigned long flags             void *data
** 166   sys_umount2          const char *target           int flags
** 167   sys_swapon           const char *specialfile      int swap_flags
** 168   sys_swapoff          const char *specialfile
** 169   sys_reboot           int magic1                   int magic2                    unsigned int cmd                 void *arg
** 170   sys_sethostname      char *name                   int len
** 171   sys_setdomainname    char *name                   int len
** 172   sys_iopl             unsigned int level           struct pt_regs *regs
** 173   sys_ioperm           unsigned long from           unsigned long num             int turn_on
** 174   sys_create_module    REMOVED IN Linux 2.6
** 175   sys_init_module      void *umod                   unsigned long len             const char *uargs
** 176   sys_delete_module    const char *name_user        unsigned int flags
** 177   sys_get_kernel_syms  REMOVED IN Linux 2.6
** 178   sys_query_module     REMOVED IN Linux 2.6
** 179   sys_quotactl         unsigned int cmd             const char *special           qid_t id                         void *addr
** 180   sys_nfsservctl       NOT IMPLEMENTED
** 181   sys_getpmsg          NOT IMPLEMENTED
** 182   sys_putpmsg          NOT IMPLEMENTED
** 183   sys_afs_syscall      NOT IMPLEMENTED
** 184   sys_tuxcall          NOT IMPLEMENTED
** 185   sys_security         NOT IMPLEMENTED
** 186   sys_gettid
** 187   sys_readahead        int fd                       loff_t offset                 size_t count
** 188   sys_setxattr         const char *pathname         const char *name              const void *value                size_t size                    int flags
** 189   sys_lsetxattr        const char *pathname         const char *name              const void *value                size_t size                    int flags
** 190   sys_fsetxattr        int fd                       const char *name              const void *value                size_t size                    int flags
** 191   sys_getxattr         const char *pathname         const char *name              void *value                      size_t size
** 192   sys_lgetxattr        const char *pathname         const char *name              void *value                      size_t size
** 193   sys_fgetxattr        int fd                       const char *name              void *value                      size_t size
** 194   sys_listxattr        const char *pathname         char *list                    size_t size
** 195   sys_llistxattr       const char *pathname         char *list                    size_t size
** 196   sys_flistxattr       int fd                       char *list                    size_t size
** 197   sys_removexattr      const char *pathname         const char *name
** 198   sys_lremovexattr     const char *pathname         const char *name
** 199   sys_fremovexattr     int fd                       const char *name
** 200   sys_tkill            pid_t pid                    int sig
** 201   sys_time             time_t *tloc
** 202   sys_futex            u32 *uaddr                   int op                        u32 val                         struct timespec *utime           u32 *uaddr2                  u32 val3
** 203   sys_sched_setaffinity pid_t pid                   unsigned int len              unsigned long *user_mask_ptr
** 204   sys_sched_getaffinity pid_t pid                   unsigned int len              unsigned long *user_mask_ptr
** 205   sys_set_thread_area  NOT IMPLEMENTED. Use arch_prctl
** 206   sys_io_setup         unsigned nr_events           aio_context_t *ctxp
** 207   sys_io_destroy       aio_context_t ctx
** 208   sys_io_getevents     aio_context_t ctx_id         long min_nr                   long nr                         struct io_event *events
** 209   sys_io_submit        aio_context_t ctx_id         long nr                       struct iocb **iocbpp
** 210   sys_io_cancel        aio_context_t ctx_id         struct iocb *iocb             struct io_event *result
** 211   sys_get_thread_area  NOT IMPLEMENTED. Use arch_prctl
** 212   sys_lookup_dcookie   u64 cookie64                 long buf                      long len
** 213   sys_epoll_create     int size
** 214   sys_epoll_ctl_old    NOT IMPLEMENTED
** 215   sys_epoll_wait_old   NOT IMPLEMENTED
** 216   sys_remap_file_pages unsigned long start          unsigned long size            unsigned long prot               unsigned long pgoff              unsigned long flags
** 217   sys_getdents64       unsigned int fd              struct linux_dirent64 *dirent unsigned int count
** 218   sys_set_tid_address  int *tidptr
** 219   sys_restart_syscall
** 220   sys_semtimedop       int semid                    struct sembuf *tsops          unsigned nsops                   const struct timespec *timeout
** 221   sys_fadvise64        int fd                       loff_t offset                 size_t len                       int advice
** 222   sys_timer_create     const clockid_t which_clock  struct sigevent *timer_event_spec timer_t *created_timer_id
** 223   sys_timer_settime    timer_t timer_id             int flags                     const struct itimerspec *new_setting struct itimerspec *old_setting
** 224   sys_timer_gettime    timer_t timer_id             struct itimerspec *setting
** 225   sys_timer_getoverrun timer_t timer_id
** 226   sys_timer_delete     timer_t timer_id
** 227   sys_clock_settime    const clockid_t which_clock  const struct timespec *tp
** 228   sys_clock_gettime    const clockid_t which_clock  struct timespec *tp
** 229   sys_clock_getres     const clockid_t which_clock  struct timespec *tp
** 230   sys_clock_nanosleep  const clockid_t which_clock  int flags                     const struct timespec *rqtp        struct timespec *rmtp
** 231   sys_exit_group       int error_code
** 232   sys_epoll_wait       int epfd                     struct epoll_event *events    int maxevents                     int timeout
** 233   sys_epoll_ctl        int epfd                     int op                        int fd                           struct epoll_event *event
** 234   sys_tgkill           pid_t tgid                   pid_t pid                     int sig
** 235   sys_utimes           char *filename               struct timeval *utimes
** 236   sys_vserver          NOT IMPLEMENTED
** 237   sys_mbind            unsigned long start          unsigned long len             unsigned long mode                unsigned long *nmask             unsigned long maxnode         unsigned flags
** 238   sys_set_mempolicy    int mode                     unsigned long *nmask          unsigned long maxnode
** 239   sys_get_mempolicy    int *policy                  unsigned long *nmask          unsigned long maxnode             unsigned long addr               unsigned long flags
** 240   sys_mq_open          const char *u_name           int oflag                     mode_t mode                       struct mq_attr *u_attr
** 241   sys_mq_unlink        const char *u_name
** 242   sys_mq_timedsend     mqd_t mqdes                  const char *u_msg_ptr         size_t msg_len                    unsigned int msg_prio             const struct timespec *u_abs_timeout
** 243   sys_mq_timedreceive  mqd_t mqdes                  char *u_msg_ptr               size_t msg_len                    unsigned int *u_msg_prio          const struct timespec *u_abs_timeout
** 244   sys_mq_notify        mqd_t mqdes                  const struct sigevent *u_notification
** 245   sys_mq_getsetattr    mqd_t mqdes                  const struct mq_attr *u_mqstat struct mq_attr *u_omqstat
** 246   sys_kexec_load       unsigned long entry          unsigned long nr_segments     struct kexec_segment *segments    unsigned long flags
** 247   sys_waitid           int which                    pid_t upid                    struct siginfo *infop             int options                     struct rusage *ru
** 248   sys_add_key          const char *_type            const char *_description      const void *_payload              size_t plen
** 249   sys_request_key      const char *_type            const char *_description      const char *_callout_info         key_serial_t destringid
** 250   sys_keyctl           int option                   unsigned long arg2            unsigned long arg3                unsigned long arg4              unsigned long arg5
** 251   sys_ioprio_set       int which                    int who                       int ioprio
** 252   sys_ioprio_get       int which                    int who
** 253   sys_inotify_init
** 254   sys_inotify_add_watch int fd                      const char *pathname          u32 mask
** 255   sys_inotify_rm_watch int fd                       __s32 wd
** 256   sys_migrate_pages    pid_t pid                    unsigned long maxnode         const unsigned long *old_nodes    const unsigned long *new_nodes
** 257   sys_openat           int dfd                      const char *filename          int flags                         int mode
** 258   sys_mkdirat          int dfd                      const char *pathname          int mode
** 259   sys_mknodat          int dfd                      const char *filename          int mode                          unsigned dev
** 260   sys_fchownat         int dfd                      const char *filename          uid_t user                        gid_t group                     int flag
** 261   sys_futimesat        int dfd                      const char *filename          struct timeval *utimes
** 262   sys_newfstatat       int dfd                      const char *filename          struct stat *statbuf              int flag
** 263   sys_unlinkat         int dfd                      const char *pathname          int flag
** 264   sys_renameat         int oldfd                    const char *oldname           int newfd                         const char *newname
** 265   sys_linkat           int oldfd                    const char *oldname           int newfd                         const char *newname              int flags
** 266   sys_symlinkat        const char *oldname          int newfd                     const char *newname
** 267   sys_readlinkat       int dfd                      const char *pathname          char *buf                         int bufsiz
** 268   sys_fchmodat         int dfd                      const char *filename          mode_t mode
** 269   sys_faccessat        int dfd                      const char *filename          int mode
** 270   sys_pselect6         int n                        fd_set *inp                   fd_set *outp                      fd_set *exp                     struct timespec *tsp         void *sig
** 271   sys_ppoll            struct pollfd *ufds          unsigned int nfds             struct timespec *tsp              const sigset_t *sigmask          size_t sigsetsize
** 272   sys_unshare          unsigned long unshare_flags
** 273   sys_set_robust_list  struct robust_list_head *head size_t len
** 274   sys_get_robust_list  int pid                      struct robust_list_head **head_ptr size_t *len_ptr
** 275   sys_splice           int fd_in                    loff_t *off_in                int fd_out                        loff_t *off_out                  size_t len                   unsigned int flags
** 276   sys_tee              int fdin                     int fdout                     size_t len                        unsigned int flags
** 277   sys_sync_file_range  long fd                      loff_t offset                 loff_t bytes                      long flags
** 278   sys_vmsplice         int fd                       const struct iovec *iov       unsigned long nr_segs             unsigned int flags
** 279   sys_move_pages       pid_t pid                    unsigned long nr_pages        const void **pages                const int *nodes                 int *status                 int flags
** 280   sys_utimensat        int dfd                      const char *filename          struct timespec *utimes           int flags
** 281   sys_epoll_pwait      int epfd                     struct epoll_event *events    int maxevents                     int timeout                     const sigset_t *sigmask      size_t sigsetsize
** 282   sys_signalfd         int ufd                      sigset_t *user_mask           size_t sizemask
** 283   sys_timerfd_create   int clockid                  int flags
** 284   sys_eventfd          unsigned int count
** 285   sys_fallocate        long fd                      long mode                     loff_t offset                     loff_t len
** 286   sys_timerfd_settime  int ufd                      int flags                     const struct itimerspec *utmr      struct itimerspec *otmr
** 287   sys_timerfd_gettime  int ufd                      struct itimerspec *otmr
** 288   sys_accept4          int fd                       struct sockaddr *upeer_sockaddr int *upeer_addrlen               int flags
** 289   sys_signalfd4        int ufd                      sigset_t *user_mask           size_t sizemask                   int flags
** 290   sys_eventfd2         unsigned int count           int flags
** 291   sys_epoll_create1    int flags
** 292   sys_dup3             unsigned int oldfd           unsigned int newfd            int flags
** 293   sys_pipe2            int *filedes                 int flags
** 294   sys_inotify_init1    int flags
** 295   sys_preadv           unsigned long fd             const struct iovec *vec       unsigned long vlen                unsigned long pos_l               unsigned long pos_h
** 296   sys_pwritev          unsigned long fd             const struct iovec *vec       unsigned long vlen                unsigned long pos_l               unsigned long pos_h
** 297   sys_rt_tgsigqueueinfo pid_t tgid                  pid_t pid                     int sig                           siginfo_t *uinfo
** 298   sys_perf_event_open  struct perf_event_attr *attr_uptr pid_t pid                 int cpu                           int group_fd                      unsigned long flags
** 299   sys_recvmmsg         int fd                       struct msghdr *mmsg           unsigned int vlen                 unsigned int flags                struct timespec *timeout
** 300   sys_fanotify_init    unsigned int flags           unsigned int event_f_flags
** 301   sys_fanotify_mark    long fanotify_fd             long flags                    __u64 mask                        long dfd                          long pathname
** 302   sys_prlimit64        pid_t pid                    unsigned int resource         const struct rlimit64 *new_rlim    struct rlimit64 *old_rlim
** 303   sys_name_to_handle_at int dfd                     const char *name              struct file_handle *handle         int *mnt_id                       int flag
** 304   sys_open_by_handle_at int dfd                     const char *name              struct file_handle *handle         int *mnt_id                       int flags
** 305   sys_clock_adjtime    clockid_t which_clock       struct timex *tx
** 306   sys_syncfs           int fd
** 307   sys_sendmmsg         int fd                       struct mmsghdr *mmsg          unsigned int vlen                 unsigned int flags
** 308   sys_setns            int fd                       int nstype
** 309   sys_getcpu           unsigned *cpup               unsigned *nodep               struct getcpu_cache *unused
** 310   sys_process_vm_readv pid_t pid                    const struct iovec *lvec      unsigned long liovcnt             const struct iovec *rvec           unsigned long riovcnt      unsigned long flags
** 311   sys_process_vm_writev pid_t pid                   const struct iovec *lvec      unsigned long liovcnt             const struct iovcc *rvec           unsigned long riovcnt      unsigned long flags
** 312   sys_kcmp             pid_t pid1                   pid_t pid2                    int type                          unsigned long idx1                 unsigned long idx2
** 313   sys_finit_module     int fd                       const char *uargs             int flags
** 314   sys_sched_setattr    pid_t pid                    struct sched_attr *attr       unsigned int flags
** 315   sys_sched_getattr    pid_t pid                    struct sched_attr *attr       unsigned int size                 unsigned int flags
** 316   sys_renameat2        int olddfd                   const char *oldname           int newdfd                        const char *newname               unsigned int flags
** 317   sys_seccomp          unsigned int op              unsigned int flags            const char *uargs
** 318   sys_getrandom        char *buf                    size_t count                  unsigned int flags
** 319   sys_memfd_create     const char *uname_ptr        unsigned int flags
** 320   sys_kexec_file_load  int kernel_fd                int initrd_fd                 unsigned long cmdline_len         const char *cmdline_ptr           unsigned long flags
** 321   sys_bpf              int cmd                      union bpf_attr *attr          unsigned int size
** 322   stub_execveat        int dfd                      const char *filename          const char *const *argv           const char *const *envp           int flags
** 323   userfaultfd          int flags
** 324   membarrier           int cmd                      int flags
** 325   mlock2               unsigned long start          size_t len                    int flags
** 326   copy_file_range      int fd_in                    loff_t *off_in                int fd_out                        loff_t *off_out                   size_t len                 unsigned int flags
** 327   preadv2              unsigned long fd             const struct iovec *vec       unsigned long vlen                unsigned long pos_l               unsigned long pos_h        int flags
** 328   pwritev2             unsigned long fd             const struct iovec *vec       unsigned long vlen                unsigned long pos_l               unsigned long pos_h        int flags
** 329   pkey_mprotect
** 330   pkey_alloc
** 331   pkey_free
** 332   statx
** 333   io_pgetevents
** 334   rseq
** 335   pkey_mprotect
**
**/

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_STAT 4
#define SYS_FSTAT 5
#define SYS_LSTAT 6
#define SYS_POLL 7
#define SYS_LSEEK 8
#define SYS_MMAP 9
#define SYS_MPROTECT 10
#define SYS_MUNMAP 11
#define SYS_BRK 12
#define SYS_RT_SIGACTION 13
#define SYS_RT_SIGPROCMASK 14
#define SYS_RT_SIGRETURN 15
#define SYS_IOCTL 16
#define SYS_PREAD64 17
#define SYS_PWRITE64 18
#define SYS_READV 19
#define SYS_WRITEV 20
#define SYS_ACCESS 21
#define SYS_PIPE 22
#define SYS_SELECT 23
#define SYS_SCHED_YIELD 24
#define SYS_MREMAP 25
#define SYS_MSYNC 26
#define SYS_MINCORE 27
#define SYS_MADVISE 28
#define SYS_SHMGET 29
#define SYS_SHMAT 30
#define SYS_SHMCTL 31
#define SYS_DUP 32
#define SYS_DUP2 33
#define SYS_PAUSE 34
#define SYS_NANOSLEEP 35
#define SYS_GETITIMER 36
#define SYS_ALARM 37
#define SYS_SETITIMER 38
#define SYS_GETPID 39
#define SYS_SENDFILE 40
#define SYS_SOCKET 41
#define SYS_CONNECT 42
#define SYS_ACCEPT 43
#define SYS_SENDTO 44
#define SYS_RECVFROM 45
#define SYS_SENDMSG 46
#define SYS_RECVMSG 47
#define SYS_SHUTDOWN 48
#define SYS_BIND 49
#define SYS_LISTEN 50
#define SYS_GETSOCKNAME 51
#define SYS_GETPEERNAME 52
#define SYS_SOCKETPAIR 53
#define SYS_SETSOCKOPT 54
#define SYS_GETSOCKOPT 55
#define SYS_CLONE 56
#define SYS_FORK 57
#define SYS_VFORK 58
#define SYS_EXECVE 59
#define SYS_EXIT 60
#define SYS_WAIT4 61
#define SYS_KILL 62
#define SYS_UNAME 63
#define SYS_SEMGET 64
#define SYS_SEMOP 65
#define SYS_SEMCTL 66
#define SYS_SHMDT 67
#define SYS_MSGGET 68
#define SYS_MSGSND 69
#define SYS_MSGRCV 70
#define SYS_MSGCTL 71
#define SYS_FCNTL 72
#define SYS_FLOCK 73
#define SYS_FSYNC 74
#define SYS_FDATASYNC 75
#define SYS_TRUNCATE 76
#define SYS_FTRUNCATE 77
#define SYS_GETDENTS 78
#define SYS_GETCWD 79
#define SYS_CHDIR 80
#define SYS_FCHDIR 81
#define SYS_RENAME 82
#define SYS_MKDIR 83
#define SYS_RMDIR 84
#define SYS_CREAT 85
#define SYS_LINK 86
#define SYS_UNLINK 87
#define SYS_SYMLINK 88
#define SYS_READLINK 89
#define SYS_CHMOD 90
#define SYS_FCHMOD 91
#define SYS_CHOWN 92
#define SYS_FCHOWN 93
#define SYS_LCHOWN 94
#define SYS_UMASK 95
#define SYS_GETTIMEOFDAY 96
#define SYS_GETRLIMIT 97
#define SYS_GETRUSAGE 98
#define SYS_SYSINFO 99
#define SYS_TIMES 100
#define SYS_PTRACE 101
#define SYS_GETUID 102
#define SYS_SYSLOG 103
#define SYS_GETGID 104
#define SYS_SETUID 105
#define SYS_SETGID 106
#define SYS_GETEUID 107
#define SYS_GETEGID 108
#define SYS_SETPGID 109
#define SYS_GETPPID 110
#define SYS_GETPGRP 111
#define SYS_SETSID 112
#define SYS_SETREUID 113
#define SYS_SETREGID 114
#define SYS_GETGROUPS 115
#define SYS_SETGROUPS 116
#define SYS_SETRESUID 117
#define SYS_GETRESUID 118
#define SYS_SETRESGID 119
#define SYS_GETRESGID 120
#define SYS_GETPGID 121
#define SYS_SETFSUID 122
#define SYS_SETFSGID 123
#define SYS_GETSID 124
#define SYS_CAPGET 125
#define SYS_CAPSET 126
#define SYS_RT_SIGPENDING 127
#define SYS_RT_SIGTIMEDWAIT 128
#define SYS_RT_SIGQUEUEINFO 129
#define SYS_RT_SIGSUSPEND 130
#define SYS_SIGALTSTACK 131
#define SYS_UTIME 132
#define SYS_MKNOD 133
#define SYS_USELIB 134
#define SYS_PERSONALITY 135
#define SYS_USTAT 136
#define SYS_STATFS 137
#define SYS_FSTATFS 138
#define SYS_SYSFS 139
#define SYS_GETPRIORITY 140
#define SYS_SETPRIORITY 141
#define SYS_SCHED_SETPARAM 142
#define SYS_SCHED_GETPARAM 143
#define SYS_SCHED_SETSCHEDULER 144
#define SYS_SCHED_GETSCHEDULER 145
#define SYS_SCHED_GET_PRIORITY_MAX 146
#define SYS_SCHED_GET_PRIORITY_MIN 147
#define SYS_SCHED_RR_GET_INTERVAL 148
#define SYS_MLOCK 149
#define SYS_MUNLOCK 150
#define SYS_MLOCKALL 151
#define SYS_MUNLOCKALL 152
#define SYS_VHANGUP 153
#define SYS_MODIFY_LDT 154
#define SYS_PIVOT_ROOT 155
#define SYS__SYSCTL 156
#define SYS_PRCTL 157
#define SYS_ARCH_PRCTL 158
#define SYS_ADJTIMEX 159
#define SYS_SETRLIMIT 160
#define SYS_CHROOT 161
#define SYS_SYNC 162
#define SYS_ACCT 163
#define SYS_SETTIMEOFDAY 164
#define SYS_MOUNT 165
#define SYS_UMOUNT2 166
#define SYS_SWAPON 167
#define SYS_SWAPOFF 168
#define SYS_REBOOT 169
#define SYS_SETHOSTNAME 170
#define SYS_SETDOMAINNAME 171
#define SYS_IOPL 172
#define SYS_IOPERM 173
#define SYS_CREATE_MODULE 174
#define SYS_INIT_MODULE 175
#define SYS_DELETE_MODULE 176
#define SYS_GET_KERNEL_SYMS 177
#define SYS_QUERY_MODULE 178
#define SYS_QUOTACTL 179
#define SYS_NFSSERVCTL 180
#define SYS_GETPMSG 181
#define SYS_PUTPMSG 182
#define SYS_AFS_SYSCALL 183
#define SYS_TUXCALL 184
#define SYS_SECURITY 185
#define SYS_GETTID 186
#define SYS_READAHEAD 187
#define SYS_SETXATTR 188
#define SYS_LSETXATTR 189
#define SYS_FSETXATTR 190
#define SYS_GETXATTR 191
#define SYS_LGETXATTR 192
#define SYS_FGETXATTR 193
#define SYS_LISTXATTR 194
#define SYS_LLISTXATTR 195
#define SYS_FLISTXATTR 196
#define SYS_REMOVEXATTR 197
#define SYS_LREMOVEXATTR 198
#define SYS_FREMOVEXATTR 199
#define SYS_TKILL 200
#define SYS_TIME 201
#define SYS_FUTEX 202
#define SYS_SCHED_SETAFFINITY 203
#define SYS_SCHED_GETAFFINITY 204
#define SYS_SET_THREAD_AREA 205
#define SYS_IO_SETUP 206
#define SYS_IO_DESTROY 207
#define SYS_IO_GETEVENTS 208
#define SYS_IO_SUBMIT 209
#define SYS_IO_CANCEL 210
#define SYS_GET_THREAD_AREA 211
#define SYS_LOOKUP_DCOOKIE 212
#define SYS_EPOLL_CREATE 213
#define SYS_EPOLL_CTL_OLD 214
#define SYS_EPOLL_WAIT_OLD 215
#define SYS_REMAP_FILE_PAGES 216
#define SYS_GETDENTS64 217
#define SYS_SET_TID_ADDRESS 218
#define SYS_RESTART_SYSCALL 219
#define SYS_SEMTIMEDOP 220
#define SYS_FADVISE64 221
#define SYS_TIMER_CREATE 222
#define SYS_TIMER_SETTIME 223
#define SYS_TIMER_GETTIME 224
#define SYS_TIMER_GETOVERRUN 225
#define SYS_TIMER_DELETE 226
#define SYS_CLOCK_SETTIME 227
#define SYS_CLOCK_GETTIME 228
#define SYS_CLOCK_GETRES 229
#define SYS_CLOCK_NANOSLEEP 230
#define SYS_EXIT_GROUP 231
#define SYS_EPOLL_WAIT 232
#define SYS_EPOLL_CTL 233
#define SYS_TGKILL 234
#define SYS_UTIMES 235
#define SYS_VSERVER 236
#define SYS_MBIND 237
#define SYS_SET_MEMPOLICY 238
#define SYS_GET_MEMPOLICY 239
#define SYS_MQ_OPEN 240
#define SYS_MQ_UNLINK 241
#define SYS_MQ_TIMEDSEND 242
#define SYS_MQ_TIMEDRECEIVE 243
#define SYS_MQ_NOTIFY 244
#define SYS_MQ_GETSETATTR 245
#define SYS_KEXEC_LOAD 246
#define SYS_WAITID 247
#define SYS_ADD_KEY 248
#define SYS_REQUEST_KEY 249
#define SYS_KEYCTL 250
#define SYS_IOPRIO_SET 251
#define SYS_IOPRIO_GET 252
#define SYS_INOTIFY_INIT 253
#define SYS_INOTIFY_ADD_WATCH 254
#define SYS_INOTIFY_RM_WATCH 255
#define SYS_MIGRATE_PAGES 256
#define SYS_OPENAT 257
#define SYS_MKDIRAT 258
#define SYS_MKNODAT 259
#define SYS_FCHOWNAT 260
#define SYS_FUTIMESAT 261
#define SYS_NEWFSTATAT 262
#define SYS_UNLINKAT 263
#define SYS_RENAMEAT 264
#define SYS_LINKAT 265
#define SYS_SYMLINKAT 266
#define SYS_READLINKAT 267
#define SYS_FCHMODAT 268
#define SYS_FACCESSAT 269
#define SYS_PSELECT6 270
#define SYS_PPOLL 271
#define SYS_UNSHARE 272
#define SYS_SET_ROBUST_LIST 273
#define SYS_GET_ROBUST_LIST 274
#define SYS_SPLICE 275
#define SYS_TEE 276
#define SYS_SYNC_FILE_RANGE 277
#define SYS_VMSPLICE 278
#define SYS_MOVE_PAGES 279
#define SYS_UTIMENSAT 280
#define SYS_EPOLL_PWAIT 281
#define SYS_SIGNALFD 282
#define SYS_TIMERFD_CREATE 283
#define SYS_EVENTFD 284
#define SYS_FALLOCATE 285
#define SYS_TIMERFD_SETTIME 286
#define SYS_TIMERFD_GETTIME 287
#define SYS_ACCEPT4 288
#define SYS_SIGNALFD4 289
#define SYS_EVENTFD2 290
#define SYS_EPOLL_CREATE1 291
#define SYS_DUP3 292
#define SYS_PIPE2 293
#define SYS_INOTIFY_INIT1 294
#define SYS_PREADV 295
#define SYS_PWRITEV 296
#define SYS_RT_TGSIGQUEUEINFO 297
#define SYS_PERF_EVENT_OPEN 298
#define SYS_RECVMMSG 299
#define SYS_FANOTIFY_INIT 300
#define SYS_FANOTIFY_MARK 301
#define SYS_PRLIMIT64 302
#define SYS_NAME_TO_HANDLE_AT 303
#define SYS_OPEN_BY_HANDLE_AT 304
#define SYS_CLOCK_ADJTIME 305
#define SYS_SYNCFS 306
#define SYS_SENDMMSG 307
#define SYS_SETNS 308
#define SYS_GETCPU 309
#define SYS_PROCESS_VM_READV 310
#define SYS_PROCESS_VM_WRITEV 311
#define SYS_KCMP 312
#define SYS_FINIT_MODULE 313
#define SYS_SCHED_SETATTR 314
#define SYS_SCHED_GETATTR 315
#define SYS_RENAMEAT2 316
#define SYS_SECCOMP 317
#define SYS_GETRANDOM 318
#define SYS_MEMFD_CREATE 319
#define SYS_KEXEC_FILE_LOAD 320
#define SYS_BPF 321
#define STUB_EXECVEAT 322
#define SYS_USERFAULTFD 323
#define SYS_MEMBARRIER 324
#define SYS_MLOCK2 325
#define SYS_COPY_FILE_RANGE 326
#define SYS_PREADV2 327
#define SYS_PWRITEV2 328
#define SYS_PKEY_MPROTECT 329
#define SYS_PKEY_ALLOC 330
#define SYS_PKEY_FREE 331
#define SYS_STATX 332
#define SYS_IO_PGETEVENTS 333
#define SYS_RSEQ 334
// #define SYS_PKEY_MPROTECT 335

#endif /* SYSCALL_H */
