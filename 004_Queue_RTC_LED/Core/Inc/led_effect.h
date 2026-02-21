//
// Created by hugo-juarez on 2/21/26.
//

#ifndef INC_004_QUEUE_RTC_LED_LED_EFFECT_H
#define INC_004_QUEUE_RTC_LED_LED_EFFECT_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "timers.h"

// LED APIs
void led_init_timer(void);
void led_effect_stop(void);
void led_effect(uint8_t effect);

// LED Timer Callback
void led_effect_callback(TimerHandle_t xTimer);

#endif //INC_004_QUEUE_RTC_LED_LED_EFFECT_H