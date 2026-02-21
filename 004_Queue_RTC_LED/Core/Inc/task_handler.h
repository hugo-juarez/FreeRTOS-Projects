//
// Created by hugo-juarez on 2/20/26.
//

#ifndef INC_004_QUEUE_RTC_LED_TASK_HANDLER_H
#define INC_004_QUEUE_RTC_LED_TASK_HANDLER_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Enums
typedef enum
{
    State_MainMenu = 0,
    State_LedEffect,
    State_RtcMenu,
    State_RtcTimeConfig,
    State_RtcDateConfig,
    State_RtcReport,
} State_t;

// Structs
typedef struct
{
    uint8_t payload[10];
    uint8_t len;
} Command_t;

// Task handler Getters
TaskHandle_t* get_menu_task(void);
TaskHandle_t* get_led_task(void);
TaskHandle_t* get_rtc_task(void);
TaskHandle_t* get_command_task(void);
TaskHandle_t* get_print_task(void);

// Task function handlers
void menu_handler(void* params);
void led_handler(void* params);
void rtc_handler(void* params);
void command_handler(void* params);
void print_handler(void* params);

// Queue APIs
void init_queues(void);
QueueHandle_t get_print_queue(void);
QueueHandle_t get_input_data_queue(void);

#endif //INC_004_QUEUE_RTC_LED_TASK_HANDLER_H