/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:48:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/11 14:00:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/time.h>
#include <system/cmos.h>

uint64_t startup_time = 0;
tm_t startup_tm;

int month[12] = {
    0,
    __DAY * (31),
    __DAY * (31 + 29),
    __DAY * (31 + 29 + 31),
    __DAY * (31 + 29 + 31 + 30),
    __DAY * (31 + 29 + 31 + 30 + 31),
    __DAY * (31 + 29 + 31 + 30 + 31 + 30),
    __DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31),
    __DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
    __DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    __DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    __DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)};

uint64_t mktime(tm_t *time)
{
    uint64_t __time = 0;
    uint32_t __year = 0;

    __year = time->year - 70;
    __time = __YEAR * __year + __DAY * ((__year + 1) / 4);
    __time += month[time->month];
    if (time->month > 1 && ((__year + 2) % 4))
        __time -= __DAY;
    __time += __DAY * (time->day - 1);
    __time += __HOUR * time->hours;
    __time += __MINUTE * time->minutes;
    __time += time->seconds;
    return __time;
}

tm_t gettime(void)
{
    tm_t __current_time;

    do
    {
        __current_time.seconds = cmos_read(CMOS_SECONDS);
        __current_time.minutes = cmos_read(CMOS_MINUTES);
        __current_time.hours = cmos_read(CMOS_HOURS);
        __current_time.day = cmos_read(CMOS_DAY);
        __current_time.month = cmos_read(CMOS_MONTH);
        __current_time.year = cmos_read(CMOS_YEAR);
    } while (__current_time.seconds != (uint32_t)cmos_read(CMOS_SECONDS));

    BCD_TO_BIN(__current_time.seconds);
    BCD_TO_BIN(__current_time.minutes);
    BCD_TO_BIN(__current_time.hours);
    BCD_TO_BIN(__current_time.day);
    BCD_TO_BIN(__current_time.month);
    BCD_TO_BIN(__current_time.year);

    return __current_time;
}

uint64_t difftime(tm_t *time1, tm_t *time2)
{
    return (mktime(time1) - mktime(time2));
}

uint64_t mkdifftime(uint64_t time1, uint64_t time2)
{
    printk("Time1: %u\n", time1);
    printk("Time2: %u\n", time2);
    return (time1 - time2);
}

char *asctime(tm_t *time)
{
    static char buf[26];
    static char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    char itoa_buffer[__ITOA_BUFFER_LENGTH__];
    bzero(buf, 26);
    memset(buf, ' ', 25);

    printk("Day: %d\n", time->day);
    memjoin(buf, days[(time->day + 3) % 7], 0, 3);
    memjoin(buf, " ", 3, 1);

    printk("Month: %d\n", time->month);
    memjoin(buf, months[(time->month - 1) % 12], 4, 3);
    memjoin(buf, " ", 7, 1);

    printk("Day: %d\n", time->day);
    bzero(itoa_buffer, __ITOA_BUFFER_LENGTH__);
    itoa(time->day, itoa_buffer);
    memjoin(buf, itoa_buffer, 8, 2);

    memjoin(buf, " ", 10, 1);

    bzero(itoa_buffer, __ITOA_BUFFER_LENGTH__);
    itoa(time->hours, itoa_buffer);
    memjoin(buf, itoa_buffer, 11, 2);

    memjoin(buf, ":", 13, 1);

    bzero(itoa_buffer, __ITOA_BUFFER_LENGTH__);
    itoa(time->minutes, itoa_buffer);
    memjoin(buf, itoa_buffer, 14, 2);

    memjoin(buf, ":", 16, 1);

    bzero(itoa_buffer, __ITOA_BUFFER_LENGTH__);
    itoa(time->seconds, itoa_buffer);
    memjoin(buf, itoa_buffer, 17, 2);

    memjoin(buf, " ", 19, 1);

    bzero(itoa_buffer, __ITOA_BUFFER_LENGTH__);
    itoa(time->year, itoa_buffer);
    memjoin(buf, itoa_buffer, 20, 4);

    // sprintf(buf, "%s %s %d %d:%d:%d %d",! days[time->day], months[time->month], time->day, time->hours, time->minutes, time->seconds, time->year);
    return buf;
}

void time_init(void)
{
    tm_t time;

    time = gettime();
    startup_tm = time;
    startup_time = mktime(&time);
}