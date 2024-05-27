/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:45:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/05/27 16:55:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIME_H
#define TIME_H

#include <kernel.h>

// Constants representing the number of seconds in a minute, hour, day, and year
#define __MINUTE 60
#define __HOUR (60 * __MINUTE)
#define __DAY (24 * __HOUR)
#define __YEAR (365 * __DAY)

#define CLOCK_PER_SEC 100 // Number of clock ticks per second

#define CMOS_SECONDS 0x00
#define CMOS_MINUTES 0x02
#define CMOS_HOURS 0x04
#define CMOS_DAY 0x07
#define CMOS_MONTH 0x08
#define CMOS_YEAR 0x09

// Structure representing time
typedef struct tm {
    uint32_t seconds;
    uint32_t minutes;
    uint32_t hours;
    uint32_t day;
    uint32_t month;
    uint32_t year;
    uint32_t wday;
    uint32_t yday;
    uint32_t isdst;
} tm_t;

// Global variables for storing startup time
extern uint64_t startup_time;
extern tm_t startup_tm;

// Function declarations
tm_t gettime(void);
void time_init(void);
uint64_t mktime(const tm_t *time);
char *asctime(const tm_t *timeptr);
uint64_t difftime(const tm_t *time1, const tm_t *time2);
uint64_t mkdifftime(uint64_t time1, uint64_t time2);
uint64_t get_system_time(void);
bool time_is_init(void);

#endif /* !TIME_H */