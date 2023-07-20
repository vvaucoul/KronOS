/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:45:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/07/20 18:03:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TIME_H
#define TIME_H

#include <kernel.h>

/*******************************************************************************
 *                                TIME DEFINES                                 *
 ******************************************************************************/

#define __MINUTE 0x3C            // 60
#define __HOUR (0x3C * __MINUTE) // 3600
#define __DAY (0x18 * __HOUR)    // 86400
#define __YEAR (0x16D * __DAY)   // 31536000

#define CLOCK_PER_SEC 100

#define CMOS_SECONDS 0x00
#define CMOS_MINUTES 0x02
#define CMOS_HOURS 0x04
#define CMOS_DAY 0x07
#define CMOS_MONTH 0x08
#define CMOS_YEAR 0x09

/*******************************************************************************
 *                               TIME STRUCTURE                                *
 ******************************************************************************/

typedef uint64_t clock_t;

typedef struct tm {
    uint32_t seconds; // Seconds
    uint32_t minutes; // Minutes
    uint32_t hours;   // Hours
    uint32_t day;     // Day of the month
    uint32_t month;   // Month
    uint32_t year;    // Year
    uint32_t wday;    // Day of the week
    uint32_t yday;    // Day in the year
    uint32_t isdst;   // Daylight saving time
} tm_t;

/*******************************************************************************
 *                               TIME VARIABLES                                *
 ******************************************************************************/

extern uint64_t startup_time;
extern tm_t startup_tm;
extern int month[12];

/*******************************************************************************
 *                               TIME FUNCTIONS                                *
 ******************************************************************************/

extern tm_t gettime(void);
extern void time_init(void);
extern uint64_t mktime(tm_t *time);
extern char *asctime(tm_t *timeptr);
extern uint64_t difftime(tm_t *time1, tm_t *time2);
extern uint64_t mkdifftime(uint64_t time1, uint64_t time2);
extern uint64_t get_system_time(void);

#endif /* !TIME_H */