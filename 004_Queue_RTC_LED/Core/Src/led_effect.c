//
// Created by hugo-juarez on 2/21/26.
//

#include "led_effect.h"
#include "main.h"

// Private global variables
static TimerHandle_t led_timer_handle;
static uint8_t led_effect_id;
static uint8_t flag;

// LED APIs
void led_init_timer(void)
{
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    led_timer_handle = xTimerCreate("LED Timer", xDelay, pdTRUE, 0, led_effect_callback);
}

void led_effect_stop(void)
{
    turn_off_all_leds();
    xTimerStop(led_timer_handle, portMAX_DELAY);
}

void set_led_effect(uint8_t effect)
{
    led_effect_id = effect;
    flag = 0;

    if (xTimerIsTimerActive(led_timer_handle) == pdFALSE)
    {
        xTimerStart(led_timer_handle, portMAX_DELAY);
    }
}

// LED Timer Callback
void led_effect_callback(TimerHandle_t xTimer)
{
    switch (led_effect_id)
    {
    case 1:
        (flag ^= 1) ? turn_off_all_leds() : turn_on_all_leds();
        break;
    case 2:
        (flag ^= 1) ? turn_on_even_leds() : turn_on_odd_leds();
        break;
    case 3:
        flag = (flag + 1) % 4;
        led_control( 0x1 << flag);
        break;
    case 4:
        flag = (flag + 1) % 4;
        led_control( 0x08 >> flag);
        break;
    default:
        break;
    }
}

// Helper functions
void turn_off_all_leds(void)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin,GPIO_PIN_RESET);
}

void turn_on_all_leds(void)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin,GPIO_PIN_SET);
}

void turn_on_even_leds(void)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin,GPIO_PIN_SET);
}

void turn_on_odd_leds(void)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin,GPIO_PIN_RESET);
}

void led_control(uint8_t value)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, (value >> 1) & 0x1);
    HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, (value >> 0) & 0x1);
    HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, (value >> 2) & 0x1);
    HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, (value >> 3) & 0x1);
}