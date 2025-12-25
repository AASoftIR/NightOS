/*
 * NightOS - Real-Time Clock Implementation
 * 
 * CMOS RTC driver for system date/time
 */

#include "../include/rtc.h"
#include "../include/io.h"
#include "../include/string.h"

/* Day names */
static const char* day_names[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", 
    "Thursday", "Friday", "Saturday"
};

/* Month names */
static const char* month_names[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

/* Read a byte from CMOS */
static uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

/* Check if RTC update is in progress */
static bool rtc_update_in_progress(void) {
    outb(CMOS_ADDRESS, CMOS_STATUS_A);
    return (inb(CMOS_DATA) & 0x80) != 0;
}

/* Convert BCD to binary */
static uint8_t bcd_to_binary(uint8_t bcd) {
    return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

/* Initialize RTC */
void rtc_init(void) {
    /* Nothing special needed for basic RTC reading */
}

/* Read current time from RTC */
void rtc_read_time(rtc_time_t* time) {
    /* Wait for any update to complete */
    while (rtc_update_in_progress());
    
    /* Read raw values */
    uint8_t second = cmos_read(CMOS_SECONDS);
    uint8_t minute = cmos_read(CMOS_MINUTES);
    uint8_t hour = cmos_read(CMOS_HOURS);
    uint8_t day = cmos_read(CMOS_DAY);
    uint8_t month = cmos_read(CMOS_MONTH);
    uint8_t year = cmos_read(CMOS_YEAR);
    uint8_t weekday = cmos_read(CMOS_WEEKDAY);
    
    /* Read status register B to check format */
    uint8_t status_b = cmos_read(CMOS_STATUS_B);
    
    /* Convert BCD to binary if needed */
    if (!(status_b & 0x04)) {
        second = bcd_to_binary(second);
        minute = bcd_to_binary(minute);
        hour = bcd_to_binary(hour & 0x7F) | (hour & 0x80);
        day = bcd_to_binary(day);
        month = bcd_to_binary(month);
        year = bcd_to_binary(year);
    }
    
    /* Convert 12-hour to 24-hour if needed */
    if (!(status_b & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }
    
    /* Fill structure */
    time->second = second;
    time->minute = minute;
    time->hour = hour;
    time->day = day;
    time->month = month;
    time->year = 2000 + year;  /* Assume 21st century */
    time->weekday = weekday;
}

/* Get individual components */
uint8_t rtc_get_seconds(void) {
    rtc_time_t time;
    rtc_read_time(&time);
    return time.second;
}

uint8_t rtc_get_minutes(void) {
    rtc_time_t time;
    rtc_read_time(&time);
    return time.minute;
}

uint8_t rtc_get_hours(void) {
    rtc_time_t time;
    rtc_read_time(&time);
    return time.hour;
}

uint8_t rtc_get_day(void) {
    rtc_time_t time;
    rtc_read_time(&time);
    return time.day;
}

uint8_t rtc_get_month(void) {
    rtc_time_t time;
    rtc_read_time(&time);
    return time.month;
}

uint16_t rtc_get_year(void) {
    rtc_time_t time;
    rtc_read_time(&time);
    return time.year;
}

/* Format time as HH:MM:SS */
void rtc_format_time(rtc_time_t* time, char* buffer) {
    buffer[0] = '0' + (time->hour / 10);
    buffer[1] = '0' + (time->hour % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (time->minute / 10);
    buffer[4] = '0' + (time->minute % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (time->second / 10);
    buffer[7] = '0' + (time->second % 10);
    buffer[8] = '\0';
}

/* Format date as YYYY-MM-DD */
void rtc_format_date(rtc_time_t* time, char* buffer) {
    buffer[0] = '0' + (time->year / 1000);
    buffer[1] = '0' + ((time->year / 100) % 10);
    buffer[2] = '0' + ((time->year / 10) % 10);
    buffer[3] = '0' + (time->year % 10);
    buffer[4] = '-';
    buffer[5] = '0' + (time->month / 10);
    buffer[6] = '0' + (time->month % 10);
    buffer[7] = '-';
    buffer[8] = '0' + (time->day / 10);
    buffer[9] = '0' + (time->day % 10);
    buffer[10] = '\0';
}

/* Get day name */
const char* rtc_day_name(uint8_t day) {
    if (day >= 1 && day <= 7) {
        return day_names[day - 1];
    }
    return "Unknown";
}

/* Get month name */
const char* rtc_month_name(uint8_t month) {
    if (month >= 1 && month <= 12) {
        return month_names[month - 1];
    }
    return "Unknown";
}
