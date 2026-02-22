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
void rtc_config_time(RTC_TimeTypeDef *time);
void rtc_config_date(RTC_DateTypeDef *date);

// RTC APIs
void rtc_get_time(RTC_TimeTypeDef *time);
void rtc_get_date(RTC_DateTypeDef *date);