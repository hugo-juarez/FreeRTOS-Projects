//
// Created by hugo-juarez on 2/22/26.
//

#ifndef INC_004_QUEUE_RTC_LED_RTC_H
#define INC_004_QUEUE_RTC_LED_RTC_H

#include "stm32f4xx_hal.h"

// Set RTC handler
int set_rtc_handler(RTC_HandleTypeDef* p_hrtc);

// RTC Configurations Functions
void rtc_config_time(RTC_TimeTypeDef *time);
void rtc_config_date(RTC_DateTypeDef *date);

// RTC APIs
void rtc_get_time(RTC_TimeTypeDef *time);
void rtc_get_date(RTC_DateTypeDef *date);


#endif //INC_004_QUEUE_RTC_LED_RTC_H