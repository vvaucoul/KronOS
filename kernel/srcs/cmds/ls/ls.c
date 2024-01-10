/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ls.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 17:38:31 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 20:41:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cmds/ls.h>

#include <filesystem/ext2/ext2.h>

/**
 * @brief List files in directory
 *
 * @param argc
 * @param argv
 * @return int
 *
 * @note
 * - ls : list files in current directory
 * basic implementation of ls system command
 */
int ls(int argc, char **argv) {
    (void)argc;
    (void)argv;

    return (0);
}