/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 09:59:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/19 22:11:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_H
#define SIGNAL_H

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   SIGNAL LIST                                  ||
// ! ||--------------------------------------------------------------------------------||

/*
Signal        x86/ARM     Alpha/   MIPS   PARISC   Notes most others   SPARC
─────────────────────────────────────────────────────────────────────────────
SIGHUP           1           1       1       1
SIGINT           2           2       2       2
SIGQUIT          3           3       3       3
SIGILL           4           4       4       4
SIGTRAP          5           5       5       5
SIGABRT          6           6       6       6
SIGIOT           6           6       6       6
SIGBUS           7          10      10      10
SIGEMT           -           7       7      -
SIGFPE           8           8       8       8
SIGKILL          9           9       9       9
SIGUSR1         10          30      16      16
SIGSEGV         11          11      11      11
SIGUSR2         12          31      17      17
SIGPIPE         13          13      13      13
SIGALRM         14          14      14      14
SIGTERM         15          15      15      15
SIGSTKFLT       16          -       -        7
SIGCHLD         17          20      18      18
SIGCLD           -          -       18      -
SIGCONT         18          19      25      26
SIGSTOP         19          17      23      24
SIGTSTP         20          18      24      25
SIGTTIN         21          21      26      27
SIGTTOU         22          22      27      28
SIGURG          23          16      21      29
SIGXCPU         24          24      30      12
SIGXFSZ         25          25      31      30
SIGVTALRM       26          26      28      20
SIGPROF         27          27      29      21
SIGWINCH        28          28      20      23
SIGIO           29          23      22      22
SIGPOLL                                             Same as SIGIO
SIGPWR          30         29/-     19      19
SIGINFO          -         29/-     -       -
SIGLOST          -         -/29     -       -
SIGSYS          31          12      12      31
SIGUNUSED       31          -       -       31
*/

// ! ||--------------------------------------------------------------------------------||
// ! ||                                   SIGNAL DATA                                  ||
// ! ||--------------------------------------------------------------------------------||

typedef enum {
    SIGHUP = 1,
    SIGINT = 2,
    SIGQUIT = 3,
    SIGILL = 4,
    SIGTRAP = 5,
    SIGABRT = 6,
    SIGIOT = 6,
    SIGBUS = 7,
    SIGEMT = -1,
    SIGFPE = 8,
    SIGKILL = 9,
    SIGUSR1 = 10,
    SIGSEGV = 11,
    SIGUSR2 = 12,
    SIGPIPE = 13,
    SIGALRM = 14,
    SIGTERM = 15,
    SIGSTKFLT = 16,
    SIGCHLD = 17,
    SIGCLD = -1,
    SIGCONT = 18,
    SIGSTOP = 19,
    SIGTSTP = 20,
    SIGTTIN = 21,
    SIGTTOU = 22,
    SIGURG = 23,
    SIGXCPU = 24,
    SIGXFSZ = 25,
    SIGVTALRM = 26,
    SIGPROF = 27,
    SIGWINCH = 28,
    SIGIO = 29,
    SIGPOLL = SIGIO,
    SIGPWR = 30,
    SIGINFO = -1,
    SIGLOST = -1,
    SIGSYS = 31,
    SIGUNUSED = 31,
} signal_t;

typedef struct signal_node {
    signal_t signum;
    void (*handler)(int32_t);
    struct signal_node *next;
} signal_node_t;

typedef int32_t signal_handler_t;

extern signal_node_t *__signal_handlers;

// ! ||--------------------------------------------------------------------------------||
// ! ||                                    FUNCTIONS                                   ||
// ! ||--------------------------------------------------------------------------------||

extern void signal(int pid, int signum);
extern void init_signals(void);

#endif /* !SIGNAL_H */