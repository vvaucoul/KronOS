/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getcwd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/13 16:30:40 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/02/13 16:36:41 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GETCWD_H
#define GETCWD_H

#include <kernel.h>

/**
 * This code snippet is related to the getcwd.h file.
 * It contains functions for working with the current working directory.
 * The getcwd function is used to get the current working directory and store it in a buffer.
 * The getwd function is used to get the current working directory and store it in a buffer.
 * The get_current_dir_name function is used to get the current working directory and return it as a pointer.
 */

extern char *getcwd(char *buf, uint32_t size);
extern char *getwd(char *buf);
extern char *get_current_dir_name(void);

#endif /* !GETCWD_H */