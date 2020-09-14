#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
struct time_s
{
  uint8_t  hour;     /* hour : 0-23 */
  uint8_t  minute;   /* minute : 0-59 */
  uint8_t  second;   /* second : 0-59 */
  uint16_t year;     /* year (no limit of definition) */
  uint8_t  month;    /* month : January = 1 ... December = 12 */
  uint8_t  day;      /* day of the month : 1-31 */
  uint8_t  week_day; /* day of the week : Monday = 1 ... Sunday = 7 */
};



typedef struct rtc_date rtc_date;



void rtc_enable_wakeup_timer(void);
void rtc_disable_wakeup_timer(void);
void rtc_enable_wakeup_timer_interrupt(void);
void rtc_disable_wakeup_timer_interrupt(void);
void rtc_calendar_config(enum rcc_osc rtc_osc);
void rtc_calendar_set(struct time_s time);
struct time_s rtc_calendar_get(void);

#endif /* RTC_H */

