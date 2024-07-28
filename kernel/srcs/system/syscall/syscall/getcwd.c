/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getcwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 16:33:22 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 22:14:36 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <multitasking/process.h>
#include <syscall/syscall.h>

#include <string.h>

/**
 * @brief Get the current working directory.
 *
 * @param buf The buffer to store the current working directory.
 * @param size The size of the buffer.
 * @return char* A pointer to the buffer containing the current working directory on success, NULL on failure.
 */
char *getcwd(char *buf, uint32_t size) {
    task_t *task = get_task(getpid());

    if (task == NULL) {
        return NULL;
    }
    if (size < strlen(task->env.pwd)) {
        return NULL;
    }
    memscpy(buf, size, task->env.pwd, strlen(task->env.pwd));
    return buf;
}

/**
 * @brief Get the current working directory.
 *
 * @param buf The buffer to store the current working directory.
 * @return char* A pointer to the buffer containing the current working directory on success, NULL on failure.
 */
char *getwd(char *buf) {
    task_t *task = get_task(getpid());
    if (task == NULL) {
        return NULL;
    }
    memscpy(buf, strlen(task->env.pwd), task->env.pwd, strlen(task->env.pwd));
    return buf;
}

/**
 * @brief Get the current working directory.
 *
 * @return char* A pointer to the buffer containing the current working directory on success, NULL on failure.
 */
char *get_current_dir_name(void) {
    task_t *task = get_task(getpid());
    if (task == NULL) {
        return NULL;
    }
    char *buf = kmalloc(strlen(task->env.pwd) + 1);

    if (buf == NULL) {
        return NULL;
    } else {
        memscpy(buf, strlen(task->env.pwd), task->env.pwd, strlen(task->env.pwd));
    }

    return buf;
}
