/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setxkbmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/10 00:18:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/10 00:27:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <shell/ksh.h>

#include <drivers/keyboard.h>

static int __setxkbmap_usage(void)
{
    printk("\t   Usage: setxkbmap [language]\n");
    printk("\t   Available languages: fr, en\n");
    return (1);
}

int setxkbmap(const ksh_args_t *args)
{
    const char *__language = ksh_get_arg(args, 0);

    if (__language == NULL)
        return (__setxkbmap_usage());
    else
    {
        if (strcmp(__language, "fr") == 0)
            keyboard_set_layout(KEYBOARD_LAYOUT_FR);
        else if (strcmp(__language, "en") == 0)
            keyboard_set_layout(KEYBOARD_LAYOUT_EN);
        else
            return (__setxkbmap_usage());
    }

    return (0);
}