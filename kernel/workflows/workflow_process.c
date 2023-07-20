/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_process.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 13:04:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 15:10:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/mmap.h>
#include <multitasking/process.h>
#include <multitasking/scheduler.h>
#include <syscall/syscall.h>
#include <system/ipc.h>
#include <system/mutex.h>
#include <system/panic.h>
#include <system/pit.h>
#include <system/socket.h>
#include <workflows/workflows.h>

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

void task1() {
    for (int i = 0; i < 100; ++i) {
        mutex_lock(&mutex);
        shared_resource++;
        printk("Task 1 increased shared resource to %d\n", shared_resource);
        mutex_unlock(&mutex);
    }
}

void task2() {
    for (int i = 0; i < 100; ++i) {
        mutex_lock(&mutex);
        shared_resource--;
        printk("Task 2 decreased shared resource to %d\n", shared_resource);
        mutex_unlock(&mutex);
    }
}

void process_06(void) {
    uint32_t length = 128;
    char *mmap_addr = (char *)mmap(NULL, length, PROT_WRITE, MAP_USER);
    if (mmap_addr == (char *)-1) {
        printk("mmap failed\n");
        return;
    }

    for (uint32_t i = 0; i < length; i++) {
        mmap_addr[i] = i % 128; // Fill with some data
    }

    // Print the content of the allocated memory
    print_content(mmap_addr, length);
}

void process_05(void) {
    // page_directory_t *pd = current_directory();
    printk("- [%d] Hello from process_05 !\n", getpid());
    printk("\t- virtual_memory_info !\n");
    print_virtual_memory_info(get_task_directory());
}

void process_04(void) {
    while (1) {
        printk("- [%d] Hello from process_04 !\n", getpid());
        ksleep(1);
    }
}

void process_03(void) {
    while (1) {
        printk("- [%d] Hello from process_03 !\n", getpid());
        ksleep(1);
    }
}

void process_02(void) {
    printk("- [%d] Hello from process_02 !\n", getpid());
}

void exec_fn(uint32_t *addr, uint32_t *function, uint32_t size) {
    uint32_t *ptr = (uint32_t *)addr;
    for (uint32_t i = 0; i < size; ++i) {
        ptr[i] = function[i];
    }
}

void process_01(void) {
    while (1) {
        printk("- [%d] Hello from process_01 !\n", getpid());
        task_exit(42);
        return;
        // ksleep(1);
    }
}

void process_test(void) {
    __WORKFLOW_HEADER();

    printk("Kernel PID: %u\n", getpid());

    printk("Create task dummy\n");
    pid_t pid = init_task(task_dummy);
    printk("Task dummy pid: %d\n", pid);

    ksleep(2);

    printk("Kill task dummy\n");
    kill(pid, SIGKILL);

    ksleep(1);
    printk("Fork task from PID [%d]\n", getpid());
    pid = fork();

    if (pid == 0) {
        while (1) {
            printk("Hello from child process\n");
            ksleep(3);
            printk("Exit child process\n");
            exit(0);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);

        printk("Hello from parent process\n");
        ksleep(1);
    }

    printk(_GREEN "Task 01"_END
                  "\n");
    int32_t pid_task_01 = init_task(process_01);
    printk("Wait task 01 [%d]\n", pid_task_01);
    int32_t ret_code = task_wait(pid_task_01);
    printk("Task 01 finished with code [%d]\n", ret_code);

    printk(_GREEN "Task 02"_END
                  "\n");
    int32_t pid_task_02 = init_task(process_02);
    ksleep(3);

    printk(_GREEN "Task 03"_END
                  "\n");
    int32_t pid_task_03 = init_task(process_03);
    ksleep(3);

    printk(_GREEN "Task 03 + 04"_END
                  "\n");
    int32_t pid_task_04 = init_task(process_04);
    ksleep(3);

    printk(_GREEN "Task 03 + 04 + 05"_END
                  "\n");
    int32_t pid_task_05 = init_task(process_05);
    ksleep(3);
    // int32_t pid_task_06 = init_task(process_06);

    ksleep(3);

    pid_t pid_ipc = fork();

    if (pid_ipc == -1) {
        // Error: fork failed
        printk("Error: fork failed\n");
    } else if (pid_ipc == 0) {
        // Child process
        printk("Child process send message to parent\n");
        ipc_send(getppid(), "Hello from Child process !");
        exit(0);
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

        printk("ST [%d] | Message from child: %s\n", st, buffer);
    }

    ksleep(3);

    pid_t pid_socket = fork();

    socket_t *socket = socket_create();
    if (pid_socket == -1) {
        // Error: fork failed
        printk("Error: fork failed\n");
    } else if (pid_socket == 0) {
        // Child process
        printk("Child process send message to parent\n");
        socket_send(socket, "Hello from Child process !", 26);
        exit(0);
    } else {
        // Parent process
        char buffer[SOCKET_BUFFER_MAX];
        int ret = 0;
        do {
            bzero(buffer, SOCKET_BUFFER_MAX);
            ret = socket_receive(socket, buffer, SOCKET_BUFFER_MAX);
            printk("Waiting for message from child...\n");
            ksleep(1);
        } while (ret == 0);

        int st = 0;
        waitpid(pid_socket, &st, 0);

        printk("ST [%d] | Message from child: %s\n", st, buffer);
    }

    ksleep(3);

    // print_virtual_memory_info(current_directory);
    // kpause();

    // switch_to_user_mode();

    // TODO: fork and exec
    /* fork
    ** exec kronos_shell
    ** infinite pause
    */

    // switch to user mode

    // switch_user_mode();
    // kpause();

    // ksleep(20);

    // while (get_task(pid_task_01) != NULL && get_task(pid_task_02) != NULL && get_task(pid_task_03) != NULL && get_task(pid_task_04) != NULL && get_task(pid_task_05) != NULL) {
    //     printk("Waiting for tasks to finish...\n");
    //     kusleep(TASK_FREQUENCY);
    // }

    printk("Kill process 01 [%u]\n", pid_task_01);
    kill_task(pid_task_01);

    printk("Kill process 02 [%u]\n", pid_task_02);
    kill_task(pid_task_02);

    printk("Kill process 03 [%u]\n", pid_task_03);
    kill_task(pid_task_03);

    printk("Kill process 04 [%u]\n", pid_task_04);
    kill_task(pid_task_04);

    printk("Kill process 05 [%u]\n", pid_task_05);
    kill_task(pid_task_05);

    printk("Kill process 06 [%u]\n", pid_task_05);
    // kill_task(pid_task_06);

    printk("Kill process IPC [%u]\n", pid_ipc);
    kill_task(pid_ipc);

    printk("Kill process Socket [%u]\n", pid_socket);
    kill_task(pid_socket);

    __WORKFLOW_FOOTER();

    __UNUSED(pid_task_01);
    __UNUSED(pid_task_02);
    __UNUSED(pid_task_03);
    __UNUSED(pid_task_04);
    __UNUSED(pid_task_05);
}