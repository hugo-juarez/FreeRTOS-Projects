//
// Created by hugo-juarez on 2/22/26.
//

#include "rtc.h"
#include "FreeRTOS.h"
#include "task.h"

// Global Private Variables
static RTC_HandleTypeDef *hrtc;

// Set RTC handler
int set_rtc_handler(RTC_HandleTypeDef* p_hrtc)
{
    if (p_hrtc == NULL) return 0;
    hrtc = p_hrtc;
    return 1;
}

// RTC Configurations Functions
void rtc_config_time(RTC_TimeTypeDef *time)
{
    if (time == NULL) return;
    HAL_RTC_SetTime(hrtc, time, RTC_FORMAT_BIN);
}

void rtc_config_date(RTC_DateTypeDef *date)
{
    if (date == NULL) return;
    HAL_RTC_SetDate(hrtc, date, RTC_FORMAT_BIN);
}

// RTC APIs
void rtc_get_time(RTC_TimeTypeDef *time)
{
    HAL_RTC_GetTime(hrtc, time, RTC_FORMAT_BIN);
}

void rtc_get_date(RTC_DateTypeDef *date)
{
    HAL_RTC_GetDate(hrtc, date, RTC_FORMAT_BIN);
}