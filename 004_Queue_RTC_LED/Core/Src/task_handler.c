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

// Handlers
void menu_handler(void* params)
{

}

void led_handler(void* params)
{

}

void rtc_handler(void* params)
{

}

void command_handler(void* params)
{

}
void print_handler(void* params)
{

}