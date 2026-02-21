//
// Created by hugo-juarez on 2/20/26.
//

#ifndef INC_004_QUEUE_RTC_LED_TASK_HANDLER_H
#define INC_004_QUEUE_RTC_LED_TASK_HANDLER_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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

#endif //INC_004_QUEUE_RTC_LED_TASK_HANDLER_H