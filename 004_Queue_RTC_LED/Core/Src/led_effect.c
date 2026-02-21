//
// Created by hugo-juarez on 2/21/26.
//

#include "led_effect.h"

// Private global variables
static TimerHandle_t led_timer_handle;
static uint8_t led_effect_id;

// LED APIs
void led_init_timer(void)
{
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    led_timer_handle = xTimerCreate("LED Timer", xDelay, pdTRUE, 0, led_effect_callback);
}

void led_effect_stop(void)
{
    xTimerStop(led_timer_handle, portMAX_DELAY);
}

void set_led_effect(uint8_t effect)
{
    if (xTimerIsTimerActive(led_timer_handle) == pdFALSE)
    {
        xTimerStart(led_timer_handle, portMAX_DELAY);
    }
    led_effect_id = effect;
}

// LED Timer Callback
void led_effect_callback(TimerHandle_t xTimer)
{

}