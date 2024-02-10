/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_env.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:59:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/10 12:05:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_ENV_H
#define PROCESS_ENV_H

/**
 * @struct penv_t
 * @brief Process Environment structure containing information about the process.
 *
 * This structure holds information about the process, including the current directory.
 * It is used for managing process-specific environment variables and settings.
 */

// #warning "Replace with VFS"

#include <filesystem/vfs/vfs.h>

typedef struct s_penv {
    char pwd[64];     // Current directory (Used for PWD)
    char old_pwd[64]; // Old directory (Used for CD -)
    char home[64];    // Home directory
    char path[256];   // Path
    char *user;       // User
} __attribute__((packed)) penv_t;

#endif /* !PROCESS_ENV_H */