/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disks.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/11 11:02:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/11 18:51:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISKS_H
# define DISKS_H

#include <kernel.h>

#define CMD_DISKS_COUNT 0b00000001
#define CMD_DISKS_DETAILS 0b00000010
#define CMD_DISKS_SIZE 0b00000100
#define CMD_DISKS_INDEX 0b00001000
#define CMD_DISKS_STATE 0b00010000

typedef struct s_cmd_disks {
    uint32_t flags;
    int32_t index;
} cmd_disks_t;

extern int disks(int argc, char **argv);

#endif /* !DISKS_H */