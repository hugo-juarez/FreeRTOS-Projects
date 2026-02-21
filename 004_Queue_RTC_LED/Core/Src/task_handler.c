//
// Task Handlers
//

#include "task_handler.h"

// Private Helper functions defines
static void parse_command(void);

// Private global variables
static State_t curr_state = State_MainMenu;
static Command_t command;

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

// Task Handlers
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
        if ( xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) != pdTRUE) continue;

        if ( xQueueReceive(input_data_queue, command.payload, 0) != pdPASS) continue;

        parse_command();

        switch (curr_state)
        {
        case State_MainMenu:
            xTaskNotify(menu_task, 0, eNoAction);
            break;
        case State_LedEffect:
            xTaskNotify(led_task, 0, eNoAction);
            break;
        case State_RtcMenu:
        case State_RtcTimeConfig:
        case State_RtcDateConfig:
        case State_RtcReport:
            xTaskNotify(rtc_task, 0, eNoAction);
            break;
        default:
            break;
        }
    }
}
void print_handler(void* params)
{
    while(1)
    {
    }
}

// Private Helper functions

static void parse_command(void)
{
    int i;
    for (i = 0; i < 10; i ++)
    {

        if ( command.payload[i] == '\n')
        {
            break;
        }
    }

    command.len = --i;
    command.payload[i] = '\0';
}