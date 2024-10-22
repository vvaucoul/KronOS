/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/11 12:48:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/10/20 17:32:18 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/cmos.h>
#include <system/time.h>

#include <convert.h>
#include <macros.h>

static volatile uint64_t startup_time = 0; // Time at which the system was started
static volatile tm_t startup_tm = {0};	   // Time at which the system was started
static volatile int utc_offset = 0;		   // UTC (Coordinated Universal Time) offset in minutes

static const int month_days[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 * Sets the UTC offset for the system time.
 *
 * @param hours The number of hours to add or subtract from the current UTC time.
 * @param minutes The number of minutes to add or subtract from the current UTC time.
 */
void time_set_utc_offset(int hours, int minutes) {
	utc_offset = (hours * 60) + minutes;
}

/**
 * @brief Converts a `tm_t` structure representing a calendar time to a `uint64_t` value representing the number of seconds since the Unix epoch.
 *
 * @param time A pointer to a `tm_t` structure representing the calendar time.
 * @return The number of seconds since the Unix epoch as a `uint64_t` value.
 */
uint64_t mktime(const tm_t *time) {
	uint64_t total_time = 0;
	uint32_t year = time->year - 70;
	total_time = __YEAR * year + __DAY * ((year + 1) / 4);

	for (uint32_t i = 0; i < time->month - 1; i++) {
		total_time += __DAY * month_days[i];
	}

	if (time->month > 2 && ((year + 2) % 4 != 0)) {
		total_time -= __DAY;
	}
	total_time += __DAY * (time->day - 1);
	total_time += __HOUR * time->hours;
	total_time += __MINUTE * time->minutes;
	total_time += time->seconds;
	return total_time;
}

/**
 * @brief Retrieves the current system time.
 *
 * This function returns the current system time as a `tm_t` structure.
 *
 * @return The current system time as a `tm_t` structure.
 */
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

	// Apply UTC offset
	int total_minutes = current_time.hours * 60 + current_time.minutes + utc_offset;
	current_time.hours = (total_minutes / 60) % 24;
	current_time.minutes = total_minutes % 60;

	// Adjust the day if necessary
	if (total_minutes < 0) {
		// If total_minutes is negative, we have gone back to the previous day
		current_time.hours += 24;
		current_time.day -= 1;
		if (current_time.day == 0) {
			current_time.month -= 1;
			if (current_time.month == 0) {
				current_time.month = 12;
				current_time.year -= 1;
			}
			current_time.day = month_days[current_time.month - 1];
		}
	} else if (total_minutes >= 1440) {
		// If total_minutes exceeds the number of minutes in a day, we move to the next day
		current_time.day += 1;
		uint32_t days_in_month = month_days[current_time.month - 1];
		if (current_time.day > days_in_month) {
			current_time.day = 1;
			current_time.month += 1;
			if (current_time.month > 12) {
				current_time.month = 1;
				current_time.year += 1;
			}
		}
	}

	return current_time;
}

/**
 * Calculates the difference in time between two tm_t structures.
 *
 * @param time1 Pointer to the first tm_t structure.
 * @param time2 Pointer to the second tm_t structure.
 * @return The difference in time between time1 and time2 in milliseconds.
 */
uint64_t difftime(const tm_t *time1, const tm_t *time2) {
	if (!time_is_init()) {
		return 0;
	}
	return mktime(time1) - mktime(time2);
}

/**
 * Calculates the difference between two time values.
 *
 * @param time1 The first time value.
 * @param time2 The second time value.
 * @return The difference between time1 and time2.
 */
uint64_t mkdifftime(uint64_t time1, uint64_t time2) {
	return time1 - time2;
}

/**
 * @brief Retrieves the system time.
 *
 * This function returns the current system time as a 64-bit unsigned integer.
 *
 * @return The current system time.
 */
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

// Most powerfull code but need to implement snprintk
// char *asctime(const tm_t *time) {
//     static char buf[26];
//     static const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
//     static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

//     snprintk(buf, sizeof(buf), "%.3s %.3s %02u %02u:%02u:%02u %04u",
//              days[time->day % 7], months[time->month - 1], time->day,
//              time->hours, time->minutes, time->seconds, time->year + 1900);
//     return buf;
// }

/**
 * @brief Checks if the time module has been initialized.
 *
 * @return true if the time module has been initialized, false otherwise.
 */
bool time_is_init(void) {
	return startup_time != 0;
}

/**
 * Initializes the time module.
 */
void time_init(void) {
	tm_t time = gettime();
	startup_tm = time;
	startup_time = mktime(&time);
}

/**
 * @brief Retrieves the startup time of the system.
 *
 * This function returns the startup time of the system as a 64-bit unsigned integer.
 *
 * @return The startup time of the system.
 */
uint64_t get_startup_time(void) {
	return startup_time;
}

/**
 * @brief Retrieves the startup time.
 *
 * This function returns the startup time of the system.
 *
 * @return The startup time as a `tm_t` structure.
 */
tm_t get_startup_tm(void) {
	return startup_tm;
}