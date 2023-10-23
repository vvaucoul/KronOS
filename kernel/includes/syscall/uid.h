/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uid.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 14:20:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/23 14:30:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETUID_H
#define GETUID_H

#include <multitasking/process.h>

extern uid_t getuid(void);
extern uid_t geteuid(void);
extern void setuid(uid_t uid);
extern void seteuid(uid_t uid);

#endif /* !GETUID_H */