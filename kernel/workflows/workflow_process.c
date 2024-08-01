/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_process.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 13:04:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:17:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mm/mmap.h>
#include <mm/shared.h>
#include <multitasking/process.h>
#include <multitasking/scheduler.h>
#include <syscall/syscall.h>
#include <system/ipc.h>
#include <system/mutex.h>
#include <system/panic.h>
#include <system/pit.h>
#include <system/socket.h>
#include <workflows/workflows.h>

#include <macros.h>

extern void task_dummy(void) {
    while (1) {
        printk("Dummy task\n");
        ksleep(1);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                     PROCESS                                    ||
// ! ||--------------------------------------------------------------------------------||

void print_content(char *addr, uint32_t length) {
    printk("Content: 0x%x\n", &addr);
    for (uint32_t i = 0; i < length; i++) {
        printk("%d ", addr[i]);

        if (i % 16 == 0 && i != 0)
            printk("\n");
    }
    printk("\n");
}

void print_virtual_memory_info(page_directory_t *dir) {
    printk("Page directory: 0x%x\n", dir);
    printk("Page directory physical address: 0x%x\n", dir->physicalAddr);
    printk("Page directory virtual address: 0x%x\n", dir->tablesPhysical);
}

// Shared resource
int shared_resource = 0;

// Mutex to protect the shared resource
mutex_t mutex;

static void __orphan_03() {
    while (1) {
        printk("Hello from orphan 03 "_GREEN
               "[%d]"_END
               "\n",
               getpid());
        ksleep(1);
    }
}

static void __orphan_02() {
    init_task(__orphan_03);
    while (1) {
        printk("Hello from orphan 02 "_GREEN
               "[%d]"_END
               "\n",
               getpid());
        ksleep(1);
    }
}

static void __orphan_01() {
    init_task(__orphan_02);
    while (1) {
        printk("Hello from orphan 01 "_GREEN
               "[%d]"_END
               "\n",
               getpid());
        ksleep(1);
    }
}

void task_shared_increase() {
    for (int i = 0; i < 100; ++i) {
        mutex_lock(&mutex);
        shared_resource++;
        printk("Task 1 increased shared resource to %d\n", shared_resource);
        kmsleep(500);
        mutex_unlock(&mutex);
    }
    exit(0);
}

void task_shared_decrease() {
    for (int i = 0; i < 100; ++i) {
        mutex_lock(&mutex);
        shared_resource--;
        printk("Task 2 decreased shared resource to %d\n", shared_resource);
        kmsleep(500);
        mutex_unlock(&mutex);
    }
    exit(0);
}

void task_shared_parent(void) {
    uint32_t length = 128;

    char *mmap_addr = (char *)mmap(kmalloc(128), length, PROT_WRITE, MAP_USER);
    if (mmap_addr == (char *)-1) {
        printk("mmap failed\n");
        return;
    }

    for (uint32_t i = 0; i < length; i++) {
        mmap_addr[i] = i % 128; // Fill with some data
    }

    print_virtual_memory_info(get_task_directory());

    // Print the content of the allocated memory
    print_content(mmap_addr, length);
    exit(0);
}

void process_05(void) {
    while (1) {
        printk("- "_GREEN
               "[%d]"_END
               " Hello from process_05 !\n",
               getpid());
        ksleep(1);
    }
}

void process_04(void) {
    while (1) {
        printk("- "_GREEN
               "[%d]"_END
               " Hello from process_04 !\n",
               getpid());
        ksleep(1);
    }
}

void process_03(void) {
    while (1) {
        printk("- "_GREEN
               "[%d]"_END
               " Hello from process_03 !\n",
               getpid());
        ksleep(1);
    }
}

void process_02(void) {
    while (1) {
        printk("- "_GREEN
               "[%d]"_END
               " Hello from process_02 !\n",
               getpid());
        ksleep(1);
    }
}

void exec_fn(uint32_t *addr, uint32_t *function, uint32_t size) {
    uint32_t *ptr = (uint32_t *)addr;
    for (uint32_t i = 0; i < size; ++i) {
        ptr[i] = function[i];
    }
}

void process_01(void) {
    printk("- "_GREEN
           "[%d]"_END
           " Hello from process_01 !\n",
           getpid());
    ksleep(1);
    printk("- "_GREEN
           "[%d]"_END
           " Exit process_01 ! Return "_YELLOW
           "[%d]"_END
           "\n",
           getpid(), 42);
    exit(42);
}

static int fibonacci(int n) {
    if (n <= 1)
        return (n);
    return (fibonacci(n - 1) + fibonacci(n - 2));
}

void proc_fibo(void) {
    while (1) {
        for (int i = 0; i < 10; i++) {
            printk("["_GREEN
                   "%d"_END
                   "] Fibonacci: "_GREEN
                   "%d"_END
                   "\n",
                   getpid(), fibonacci(i));
            kmsleep(TASK_FREQUENCY);
        }
    }
}

void process_zombie_02(void) {
    while (1) {
        // printk("I'am a Zombie [%d] !\n", getpid());
        ksleep(1);
    }
}

void process_zombie_01(void) {
    while (1) {
        printk("I'am the Zombie OWNER [%d] !\n", getpid());
        // kmsleep(100);
        pid_t pid __unused__ = init_task(process_zombie_02);
    }
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                       TMP                                      ||
// ! ||--------------------------------------------------------------------------------||

void display_tasks() {
    while (1) {
        print_all_tasks();
        ksleep(3);
    }
}

void display3() {
    while (1) {
        printk("[%d] -> Coucou Louise !\n", getpid());
        ksleep(3);
    }
}

void display2() {
    while (1) {
        printk("[%d] -> Coucou papa !\n", getpid());
        ksleep(3);
    }
}

void display() {
    pid_t t __unused__ = init_task(display3);

    while (1) {
        printk("[%d] -> Coucou maman !\n", getpid());
        ksleep(3);
    }
}

void wait_pid_task(void) {
    printk("[%d] -> Hello from waitpid task\n", getpid());
    ksleep(2);
    printk("[%d] -> End waitpid task\n", getpid());
    exit(42);
}

void while_task() {
    while (1) {
        for (int i = 0; i < 10; i++) {
            printk("["_GREEN
                   "%d"_END
                   "] ["_GREEN
                   "%d"_END
                   "] Fibonacci: "_GREEN
                   "%d"_END
                   "\n",
                   getpid(), get_task(getpid())->state, fibonacci(i));
            kmsleep(TASK_FREQUENCY);
        }
    }
}

void waiting_queue_loop() {
    while (1) {
        if (get_task_count() + get_waiting_task_count() > MAX_TASKS) {
            printk("Kill task [%d]\n", get_task_count() - 1);
            kill_task(get_task_count() - 1);
        }
        kmsleep(500);
    }
}

#define OFFSET_WAITING_QUEUE 10

void ready_queue_loop() {
    while (1) {
        if (get_task_count() + get_waiting_task_count() <= MAX_TASKS + OFFSET_WAITING_QUEUE) {
            pid_t pid = init_task(task_dummy);
            printk("Create task dummy: "_GREEN
                   "[%u]"_END
                   "\n",
                   pid);
        }
        kmsleep(500);
    }
}

void tmp() {

    // init_task(ready_queue_loop);
    // init_task(waiting_queue_loop);

    init_task(task_dummy);
    init_task(task_dummy);
    init_task(task_dummy);
    init_task(task_dummy);
    

    init_task(task_dummy);
    init_task(task_dummy);
    init_task(task_dummy);
    init_task(task_dummy);

    __waiting_queue_print();




    // pid_t pid_dummy = init_task(process_zombie_01);

    // uint32_t counter = 0;
    // while (1) {
    //     init_task(process_zombie_02);
    //     printk("Create [%d] process\n", counter++);
    // }

    // printk("PID: %d\n", pid_dummy);

    ksleep(3);

    kill_task(2);
    __waiting_queue_print();
    
    // kill(pid_dummy, SIGKILL);
    // kill_all_tasks();

    pause();

    pid_t pid = init_task(wait_pid_task);
    int st = 0;
    waitpid(pid, &st, 0);
    printk("ST: %d\n", st);

    // pid_t pid = init_task(while_task);
    // ksleep(3);
    // printk("Kill task [%d]\n", pid);
    // kill_task(pid);

    // ksleep(2);

    // printk("Kill all tasks\n");

    pause();

    // init_task(display_tasks);

    // ksleep(3);

    // kill_task(2);
    // printk("Kill task 2\n");

    // pid_t pid = init_task(process_01);

    // int st2;
    // waitpid(pid, &st2, 0);
    // printk("Task ended with code: %d\n", st2);

    // ksleep(2);

    // init_task(display_tasks);

    // ksleep(2);

    // kill_task(2);

    // pause();

    // pid_t pid_01 = init_task(process_01);
    // pid_t pid_02 = init_task(display_tasks);
    // pid_t pid_03 = init_task(display);
    // pid_t pid_04 = init_task(display2);

    // ksleep(2);

    // kill_task(pid_02);

    // ksleep(2);
    // kill_task(pid_04);

    // ksleep(2);
    // kill_task(pid_03);

    // ksleep(2);
    // kill_task(pid_01);

    // ksleep(2);
    // print_all_tasks();

    // pause();

    // Check destroy page directory
    pid_t p1 = init_task(process_01);
    print_all_tasks();

    st = 0;
    waitpid(p1, &st, 0);
    printk("ST: %d\n", st);

    while (1) {
        print_all_tasks();
        ksleep(1);
    }

    pause();

    pid_t p __unused__ = init_task(display_tasks);

    pid_t t __unused__ = init_task(display);
    ksleep(1);
    pid_t e __unused__ = init_task(display2);

    ksleep(9);
    printk("Kill task [%d]\n", t);
    kill_task(t);

    // display();
    pause();
}

// ! ||--------------------------------------------------------------------------------||
// ! ||                                  PROCESS TEST                                  ||
// ! ||--------------------------------------------------------------------------------||

void process_test(void) {
    __WORKFLOW_HEADER();

    // tmp();

    printk("- Kernel PID: "_GREEN
           "[%u]"_END
           "\n",
           getpid());

    ksleep(1);

    // init_task(print_all_tasks);

    //     goto mtt;
    // mtt: {}

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                  TASK - DUMMY                                  ||
    // ! ||--------------------------------------------------------------------------------||

    pid_t pid = init_task(task_dummy);
    printk("\t- Create task dummy: "_GREEN
           "[%u]"_END
           "\n",
           pid);

    printk("- Tasks list\n");
    print_all_tasks();

    ksleep(3);

    printk("\t - Kill task dummy: "_GREEN
           "[%u]"_END
           "\n",
           pid);

    kill_task(pid);
    // kill(pid, SIGKILL);

    // pause();

    // Needed to wait for scheduler
    kmsleep(TASK_FREQUENCY);

    printk("- Tasks list\n");
    print_all_tasks();
    ksleep(1);

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                TASK - FIBONACCI                                ||
    // ! ||--------------------------------------------------------------------------------||

    printk("Create Task Fibonacci\n");

    pid_t pidfibo = init_task(proc_fibo);

    ksleep(6);
    printk("Kill Task Fibonacci\n");
    kill_task(pidfibo);

    kmsleep(TASK_FREQUENCY);

    printk("- Tasks list\n");
    print_all_tasks();

    ksleep(1);

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                MULTIPLE - TASKS                                ||
    // ! ||--------------------------------------------------------------------------------||

    printk("\n\n");
    printk("- Task "_GREEN
           "[01]"_END
           "\n");
    int32_t pid_task_01 = init_task(process_01);
    printk("Wait task "_GREEN
           "[01]"_END
           " - "_GREEN
           "[%d]"_END
           "\n",
           pid_task_01);
    int32_t ret_code = task_wait(pid_task_01);
    printk("Task "_GREEN
           "[01]"_END
           " finished with code "_YELLOW
           "[%d]"_END
           "\n",
           ret_code);

    int32_t pid_01 = init_task(process_02);
    int32_t pid_02 = init_task(process_03);
    int32_t pid_03 = init_task(process_04);
    int32_t pid_04 = init_task(process_05);

    kmsleep(TASK_FREQUENCY);
    print_all_tasks();

    ksleep(3);

    printk("- Kill task "_GREEN
           "[2]"_END
           " and "_GREEN
           "[3]"_END
           "\n");

    kmsleep(TASK_FREQUENCY);
    print_all_tasks();

    kill_task(pid_01);

    kmsleep(TASK_FREQUENCY);
    print_all_tasks();
    kill_task(pid_02);

    kmsleep(TASK_FREQUENCY);
    print_all_tasks();

    ksleep(1);

    print_all_tasks();

    printk("- Kill task "_GREEN
           "[4]"_END
           " and "_GREEN
           "[5]"_END
           "\n");

    kill_task(pid_03);
    kill_task(pid_04);

    // Needed to wait for scheduler
    ksleep(3);
    print_all_tasks();

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                               MULTIPLE TASKS - 02                              ||
    // ! ||--------------------------------------------------------------------------------||

    ksleep(3);

    printk("- Task "_GREEN
           "[02]"_END
           "\n");
    int32_t pid_task_02 = init_task(process_02);

    ksleep(3);
    printk("- Task "_GREEN
           "[03]"_END
           "\n");
    int32_t pid_task_03 = init_task(process_03);

    ksleep(3);
    printk("- Task "_GREEN
           "[04]"_END
           "\n");
    int32_t pid_task_04 = init_task(process_04);

    ksleep(3);
    printk("- Task "_GREEN
           "[05]"_END
           "\n");
    int32_t pid_task_05 = init_task(process_05);

    ksleep(6);

    // Needed to wait for scheduler
    kmsleep(TASK_FREQUENCY);
    print_all_tasks();

    // printk("Kill process 01 [%u]\n", pid_task_01);
    // kill_task(pid_task_01);

    printk("Kill process 02 [%u]\n", pid_task_02);
    kill_task(pid_task_02);
    // ksleep(1);

    printk("Kill process 03 [%u]\n", pid_task_03);
    kill_task(pid_task_03);
    // ksleep(1);

    printk("Kill process 04 [%u]\n", pid_task_04);
    kill_task(pid_task_04);
    // ksleep(1);

    printk("Kill process 05 [%u]\n", pid_task_05);
    kill_task(pid_task_05);

    ksleep(1);

    // Needed to wait for scheduler
    kmsleep(TASK_FREQUENCY);
    print_all_tasks();

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                              INTER PROCESSUS TEST                              ||
    // ! ||--------------------------------------------------------------------------------||

    printk("Inter Processus Test\n");
    ksleep(1);

    pid_t inter_pid = init_task(__orphan_01);
    ksleep(6);

    printk("Kill inter processus\n");
    kill_task(inter_pid);

    // kill_task(pid_task_06);

    ksleep(3);

    kill_all_tasks();
    // Needed to wait for scheduler
    kmsleep(TASK_FREQUENCY);
    print_all_tasks();

    ksleep(3);

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                    FORK TEST                                   ||
    // ! ||--------------------------------------------------------------------------------||

    printk("Fork task from PID [%d]\n", getpid());
    pid = fork();
    if (pid == 0) {
        printk("Hello from child process [%d]\n", getpid());
        kmsleep(500);
        printk("Exit child process\n");
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);
        printk("Hello from parent process [%d]\n", getpid());
        kmsleep(1000);
        printk("Exit parent process\n");
    }

    ksleep(3);
    kill_all_tasks();

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                               SHARED MEMORY TEST                               ||
    // ! ||--------------------------------------------------------------------------------||

    pid_t shared_01 __unused__ = init_task(task_shared_parent);
    ksleep(2);

    pid_t shared_02 __unused__ = init_task(task_shared_increase);
    pid_t shared_03 __unused__ = init_task(task_shared_decrease);

    // TMP: debug waitpid
    ksleep(3);

    // waitpid(shared_02, NULL, 0);
    // waitpid(shared_03, NULL, 0);

    printk("Result: "_GREEN
           "[%d]"_END
           "\n",
           shared_resource);

    kill_all_tasks();

    kmsleep(TASK_FREQUENCY);
    print_all_tasks();

    ksleep(3);

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                    IPC TEST                                    ||
    // ! ||--------------------------------------------------------------------------------||

    pid_t pid_ipc = fork();

    if (pid_ipc == -1) {
        // Error: fork failed
        printk("Error: fork failed\n");
    } else if (pid_ipc == 0) {
        // Child process
        printk("Child process send message to parent\n");
        ipc_send(getppid(), "Hello from Child process !");
        ksleep(2); // Must be > than parent sleep
        exit(42);
    } else {
        // Parent process
        char buffer[IPC_MSG_MAX];
        int ret = 0;
        do {
            bzero(buffer, IPC_MSG_MAX);
            ret = ipc_receive(pid_ipc, buffer);
            printk("Waiting for message from child...\n");
            ksleep(1);
        } while (ret == 0);

        int st = 0;
        waitpid(pid_ipc, &st, 0);
        // waitpid(-1, &st, 0); // both works !

        printk("ST [%d] | Message from child: %s\n", st, buffer);
    }

    ksleep(3);
    // kill_task(pid_ipc);

    // ! ||--------------------------------------------------------------------------------||
    // ! ||                                   SOCKET TEST                                  ||
    // ! ||--------------------------------------------------------------------------------||

    // Create shared socket before fork
    // socket_t *socket = (socket_t *)kmalloc_shared(sizeof(socket_t));
    socket_t *socket = socket_create(SOCKET_SHARED_DATA);

    pid_t pid_socket = fork();

    if (pid_socket == -1) {
        // Error: fork failed
        printk("Error: fork failed\n");
    } else if (pid_socket == 0) {
        // Child process
        printk("Child process send message to parent\n");
        socket_send(socket, "Hello from Child process !", 26);
        ksleep(2); // Must be > than parent sleep
        exit(1234);
    } else {
        // Parent process
        char buffer[SOCKET_BUFFER_MAX];
        int ret = 0;
        do {
            bzero(buffer, SOCKET_BUFFER_MAX);
            ret = socket_receive(socket, buffer, SOCKET_BUFFER_MAX);
            printk("Waiting for message from child... Ret: [%d]\n", ret);
            ksleep(1);
        } while (ret == 0);

        int st = 0;
        waitpid(pid_socket, &st, 0);
        // waitpid(-1, &st, 0); // both works !

        printk("ST [%d] | Message from child: %s\n", st, buffer);
    }

    ksleep(1);
    printk("- Destroying socket\n");
    socket_destroy(socket);
    ksleep(1);
    kill_all_tasks();
    print_all_tasks();
    ksleep(1);

    // Destroy shared socket

    __WORKFLOW_FOOTER();
}