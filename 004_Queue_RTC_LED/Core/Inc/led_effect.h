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
void set_led_effect(uint8_t effect);

// LED Timer Callback
void led_effect_callback(TimerHandle_t xTimer);

// Helper functions
void turn_off_all_leds(void);
void turn_on_all_leds(void);
void turn_on_even_leds(void);
void turn_on_odd_leds(void);
void led_control(uint8_t value);

#endif //INC_004_QUEUE_RTC_LED_LED_EFFECT_H