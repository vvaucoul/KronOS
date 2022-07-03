/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctypes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 12:02:29 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/06/25 12:26:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ctypes.h"

bool isalnum(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}
bool isalpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool isblank(int c)
{
    return (c == ' ' || c == '\t');
}
bool iscntrl(int c)
{
    return (c >= 0 && c <= 31) || c == 127;
}
bool isdigit(int c)
{
    return (c >= '0' && c <= '9');
}
bool isgraph(int c)
{
    return (c >= '!' && c <= '~');
}
bool islower(int c)
{
    return (c >= 'a' && c <= 'z');
}
bool isprint(int c)
{
    return (c >= ' ' && c <= '~');
}
bool ispunct(int c)
{
    return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}
bool isupper(int c)
{
    return (c >= 'A' && c <= 'Z');
}
bool isxdigit(int c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

void tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
        c += 32;
}
void toupper(int c)
{
    if (c >= 'a' && c <= 'z')
        c -= 32;
}