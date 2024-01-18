/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_env.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 15:59:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/17 20:22:11 by vvaucoul         ###   ########.fr       */
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

#include <filesystem/ext2/ext2.h>

typedef struct s_penv {
    Ext2Node *current_directory; // Current directory (Used for PWD)
} __attribute__((packed)) penv_t;

#endif /* !PROCESS_ENV_H */