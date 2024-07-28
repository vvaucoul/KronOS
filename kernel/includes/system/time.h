/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:45:33 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/29 01:07:01 by vvaucoul         ###   ########.fr       */
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

// Function declarations
extern tm_t gettime(void);
extern void time_init(void);
extern uint64_t mktime(const tm_t *time);
extern char *asctime(const tm_t *timeptr);
extern uint64_t difftime(const tm_t *time1, const tm_t *time2);
extern uint64_t mkdifftime(uint64_t time1, uint64_t time2);
extern uint64_t get_system_time(void);
extern bool time_is_init(void);
extern uint64_t get_startup_time(void);
extern tm_t get_startup_tm(void);
extern void time_set_utc_offset(int hours, int minutes);

#endif /* !TIME_H */