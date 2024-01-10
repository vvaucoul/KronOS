/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setxkbmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/10 00:18:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/09 17:13:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>

#include <drivers/keyboard.h>

static int __setxkbmap_usage(void) {
    printk("\t   Usage: setxkbmap [language]\n");
    printk("\t   Available languages: fr, en\n");
    return (1);
}

int setxkbmap(int argc, char **argv) {
    if (argc != 2) {
        return (__setxkbmap_usage());
    }

    const char *__language = argv[1];

    if (__language == NULL)
        return (__setxkbmap_usage());
    else {
        if (strcmp(__language, "fr") == 0)
            keyboard_set_layout(KEYBOARD_LAYOUT_FR);
        else if (strcmp(__language, "en") == 0)
            keyboard_set_layout(KEYBOARD_LAYOUT_EN);
        else
            return (__setxkbmap_usage());
    }

    return (0);
}