/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 10:07:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/24 01:41:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_H
#define PROCESS_H

#include <kernel.h>
#include <memory/paging.h>

#include <system/signal.h>

// #define KERNEL_STACK_SIZE 0x2000 // 2KB
// #define KERNEL_STACK_SIZE 0x8000 // 32KB

#warning "Kernel stack size is 32KB, it's too much, reduce it to 2KB"
#define KERNEL_STACK_SIZE 0x8000 // 32KB // modifier cette valeur change entierement le kernel....

#define INIT_PID 0x1 // First process pid created

// Each ticks, increase counter by ZOMBIE_HUNGRY, zombie will die after ZOMBIE_HUNGRY_DIE ticks
#define ZOMBIE_HUNGRY 0x3      // Zombie hungry counter
#define ZOMBIE_HUNGRY_DIE 0x10 // Zombie hungry die counter

typedef enum e_task_state {
    TASK_RUNNING,
    TASK_SLEEPING,
    TASK_WAITING,
    TASK_STOPPED,
    TASK_ZOMBIE,
    TASK_THREAD,
    TASK_ORPHAN,
} task_state_t;

typedef enum e_task_priority {
    TASK_PRIORITY_LOW,
    TASK_PRIORITY_MEDIUM,
    TASK_PRIORITY_HIGH,
    TASK_PRIORITY_REALTIME,
} task_priority_t;

#ifndef _PID_T
typedef int32_t pid_t;
#define _PID_T
#endif

#ifndef _UID_T
typedef uint32_t uid_t;
#define _UID_T
#endif

typedef struct s_process_cpu_load {
    uint64_t start_time;      // Time when the task started
    uint64_t load_time;       // Time when the task was loaded and finished his task
    uint64_t total_load_time; // Total time the task was loaded
} process_cpu_load_t;

typedef struct s_task {
    pid_t pid;    // Process id
    int32_t ppid; // Parent pid

    uid_t owner;           // Owner id (user id)
    uid_t effective_owner; // Effective owner id (effective user id)

    uint32_t esp, ebp; // Stack and base pointer
    uint32_t eip;      // Instruction pointer

    page_directory_t *page_directory; // Page directory

    uint32_t kernel_stack;      // Kernel stack
    struct s_task *next, *prev; // Next and previous task

    int32_t exit_code;

    uint32_t wake_up_tick; // Wake up tick (Check task sleep)

    task_priority_t or_priority; // Task priority at creation
    task_priority_t priority;    // Task priority runtime

    task_state_t state;

    process_cpu_load_t cpu_load; // CPU load (Check task cpu load)

    signal_node_t *signal_queue; // Queue of signals to be processed

    /*
    **  Task ID
    **
    **  uid: User ID
    **  gid: Group ID
    **  euid: Effective User ID
    **  egid: Effective Group ID
    */

    struct task_id_t {
        uint32_t uid;  // User ID
        uint32_t gid;  // Group ID
        uint32_t euid; // Effective User ID
        uint32_t egid; // Effective Group ID
    } task_id;

#define BSS_SIZE 0x1000
#define DATA_SIZE 0x1000

    struct sectors_t {
        void *bss_segment;
        uint32_t bss_size;
        void *data_segment;
        uint32_t data_size;
    } sectors;

    int32_t zombie_hungry; // Counter for zombie process
} task_t;

void init_tasking(void);
void switch_task(void);

int32_t task_fork(void);

int32_t getpid(void);
int32_t getppid(void);

task_t *get_current_task(void);
task_t *get_task(int32_t pid);

int32_t init_task(void func(void));

int32_t kill_task(int32_t pid);
int32_t free_task(task_t *task);
int32_t kill_all_tasks(void);
int32_t task_wait(int32_t pid);

void switch_to_user_mode(void);
void switch_user_mode(void);

extern uint32_t read_eip(void);

extern void task_exit(int32_t retval);

__attribute__((pure)) extern page_directory_t *get_task_directory(void);

task_t *get_wait_queue(void);
uint32_t get_task_count(void);
extern uint32_t getuid(void);

extern void lock_task(task_t *task);
extern void unlock_task(task_t *task);

extern void task_set_priority(pid_t pid, task_priority_t priority);

extern pid_t find_first_free_pid(void);

extern double get_cpu_load(task_t *task);

extern void print_task_info(task_t *task);
extern void print_all_tasks();
extern void print_parent_and_children(int pid);

#endif /* !PROCESS_H */