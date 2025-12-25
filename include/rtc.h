/*
 * NightOS - Real-Time Clock (RTC)
 * 
 * CMOS RTC driver for date/time
 */

#ifndef RTC_H
#define RTC_H

#include "types.h"

/* CMOS I/O Ports */
#define CMOS_ADDRESS    0x70
#define CMOS_DATA       0x71

/* CMOS Registers */
#define CMOS_SECONDS    0x00
#define CMOS_MINUTES    0x02
#define CMOS_HOURS      0x04
#define CMOS_WEEKDAY    0x06
#define CMOS_DAY        0x07
#define CMOS_MONTH      0x08
#define CMOS_YEAR       0x09
#define CMOS_CENTURY    0x32
#define CMOS_STATUS_A   0x0A
#define CMOS_STATUS_B   0x0B

/* Time structure */
typedef struct {
    uint8_t  second;
    uint8_t  minute;
    uint8_t  hour;
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
    uint8_t  weekday;
} rtc_time_t;

/* RTC functions */
void rtc_init(void);
void rtc_read_time(rtc_time_t* time);
uint8_t rtc_get_seconds(void);
uint8_t rtc_get_minutes(void);
uint8_t rtc_get_hours(void);
uint8_t rtc_get_day(void);
uint8_t rtc_get_month(void);
uint16_t rtc_get_year(void);

/* Format time as string */
void rtc_format_time(rtc_time_t* time, char* buffer);
void rtc_format_date(rtc_time_t* time, char* buffer);

/* Day of week name */
const char* rtc_day_name(uint8_t day);
const char* rtc_month_name(uint8_t month);

#endif /* RTC_H */
