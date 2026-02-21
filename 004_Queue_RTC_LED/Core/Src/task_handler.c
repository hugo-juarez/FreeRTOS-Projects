//
// Task Handlers
//

#include "task_handler.h"

// Defining task handlers
static TaskHandle_t menu_task;
static TaskHandle_t led_task;
static TaskHandle_t rtc_task;
static TaskHandle_t command_task;
static TaskHandle_t print_task;

// Task Handler Getters
TaskHandle_t* get_menu_task(void) { return &menu_task; }
TaskHandle_t* get_led_task(void) { return &led_task; }
TaskHandle_t* get_rtc_task(void) { return &rtc_task; }
TaskHandle_t* get_command_task(void) { return &command_task; }
TaskHandle_t* get_print_task(void) { return &print_task; }

// Queue Created on Main
static QueueHandle_t print_queue;
static QueueHandle_t input_data_queue;

// Queue API
void init_queues(void)
{
    print_queue = xQueueCreate(10, sizeof(char));
    configASSERT(print_queue != NULL);

    input_data_queue = xQueueCreate(10, sizeof(char));
    configASSERT(input_data_queue != NULL);
}

QueueHandle_t get_print_queue(void) { return print_queue; }
QueueHandle_t get_input_data_queue(void) { return input_data_queue; }

// Handlers
void menu_handler(void* params)
{
    while(1)
    {
    }
}

void led_handler(void* params)
{
    while(1)
    {
    }
}

void rtc_handler(void* params)
{
    while(1)
    {
    }
}

void command_handler(void* params)
{
    while(1)
    {
    }
}
void print_handler(void* params)
{
    while(1)
    {
    }
}