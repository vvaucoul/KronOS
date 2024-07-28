/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:48:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/27 19:45:28 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cmos.h>
#include <system/time.h>

#include <convert.h>

uint64_t startup_time = 0;
tm_t startup_tm = {0};

static int month[12] = {
    0,
    __DAY * 31,
    __DAY * 60,
    __DAY * 91,
    __DAY * 121,
    __DAY * 152,
    __DAY * 182,
    __DAY * 213,
    __DAY * 244,
    __DAY * 274,
    __DAY * 305,
    __DAY * 335};

uint64_t mktime(const tm_t *time) {
    uint64_t total_time = 0;
    uint32_t year = time->year - 70;

    total_time = __YEAR * year + __DAY * ((year + 1) / 4);
    total_time += month[time->month - 1];
    if (time->month > 2 && ((year + 2) % 4 != 0))
        total_time -= __DAY;
    total_time += __DAY * (time->day - 1);
    total_time += __HOUR * time->hours;
    total_time += __MINUTE * time->minutes;
    total_time += time->seconds;
    return total_time;
}

tm_t gettime(void) {
    tm_t current_time;

    do {
        current_time.seconds = cmos_read(CMOS_SECONDS);
        current_time.minutes = cmos_read(CMOS_MINUTES);
        current_time.hours = cmos_read(CMOS_HOURS);
        current_time.day = cmos_read(CMOS_DAY);
        current_time.month = cmos_read(CMOS_MONTH);
        current_time.year = cmos_read(CMOS_YEAR);
    } while (current_time.seconds != (uint32_t)cmos_read(CMOS_SECONDS));

    BCD_TO_BIN(current_time.seconds);
    BCD_TO_BIN(current_time.minutes);
    BCD_TO_BIN(current_time.hours);
    BCD_TO_BIN(current_time.day);
    BCD_TO_BIN(current_time.month);
    BCD_TO_BIN(current_time.year);

    return current_time;
}

uint64_t difftime(const tm_t *time1, const tm_t *time2) {
    if (!time_is_init())
        return 0;
    return mktime(time1) - mktime(time2);
}

uint64_t mkdifftime(uint64_t time1, uint64_t time2) {
    printk("Time1: %llu\n", time1);
    printk("Time2: %llu\n", time2);
    return time1 - time2;
}

uint64_t get_system_time(void) {
    tm_t current_time = gettime();
    return mktime(&current_time);
}

// Temporary solution
char *asctime(const tm_t *time) {
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

// Most powerfull code but need to implement snprintf
// char *asctime(const tm_t *time) {
//     static char buf[26];
//     static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
//     static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

//     snprintf(buf, sizeof(buf), "%.3s %.3s %02u %02u:%02u:%02u %04u",
//              days[time->day % 7], months[time->month - 1], time->day,
//              time->hours, time->minutes, time->seconds, time->year + 1900);
//     return buf;
// }

bool time_is_init(void) {
    return startup_time != 0;
}

void time_init(void) {
    tm_t time = gettime();
    startup_tm = time;
    startup_time = mktime(&time);
}