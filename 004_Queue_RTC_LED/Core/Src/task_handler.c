//
// Task Handlers
//

#include "task_handler.h"
#include <string.h>
#include "led_effect.h"
#include "main.h"

// Private Helper functions defines
static int parse_command(void);

// Private global variables
static State_t curr_state = State_MainMenu;
static Command_t command;
static const char* invalid_option = "////Invalid option////\n";
extern UART_HandleTypeDef huart2;

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
    print_queue = xQueueCreate(10, sizeof(size_t));
    configASSERT(print_queue != NULL);

    input_data_queue = xQueueCreate(10, sizeof(char));
    configASSERT(input_data_queue != NULL);
}

QueueHandle_t get_print_queue(void) { return print_queue; }
QueueHandle_t get_input_data_queue(void) { return input_data_queue; }

// Task Handlers
void menu_handler(void* params)
{
    uint8_t option = 2;
    const char* msg_menu = "\n========================\n"
                            "|         Menu         |\n"
                            "========================\n"
                                "LED effect    ----> 0\n"
                                "Date and time ----> 1\n"
                                "Exit          ----> 2\n"
                                "Enter your choice here : ";

    while(1)
    {
        xQueueSendToBack(print_queue, &msg_menu, portMAX_DELAY);
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        if (command.len == 1)
        {
            option = command.payload[0] - 48;
            switch (option)
            {
            case 0:
                curr_state = State_LedEffect;
                xTaskNotify(led_task, 0, eNoAction);
                break;
            case 1:
                curr_state = State_RtcMenu;
                xTaskNotify(rtc_task, 0, eNoAction);
                break;
            case 2:
                break;
            default:
                xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
                continue;
            }
        } else
        {
            xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
            continue;
        }

        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    }
}

void led_handler(void* params)
{
    const char* msg_led = "========================\n"
                          "|      LED Effect     |\n"
                          "========================\n"
                          "(none,e1,e2,e3,e4)\n"
                          "Enter your choice here : ";
    while(1)
    {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        xQueueSendToBack(print_queue, &msg_led, portMAX_DELAY);

        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        if (command.len <= 4)
        {
            if (!strcmp((char *)command.payload, "none"))
            {
                led_effect_stop();
            } else if (!strcmp((char *)command.payload, "e1"))
            {
                set_led_effect(1);
            } else if (!strcmp((char *)command.payload, "e2"))
            {
                set_led_effect(2);
            } else if (!strcmp((char *)command.payload, "e3"))
            {
                set_led_effect(3);
            } else if (!strcmp((char *)command.payload, "e4"))
            {
                set_led_effect(4);
            } else
            {
                xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
            }
        } else
        {
            xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
        }

        curr_state = State_MainMenu;

        xTaskNotify(menu_task, 0, eNoAction);
    }
}

void rtc_handler(void* params)
{
    while(1)
    {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    }
}

void command_handler(void* params)
{

    while(1)
    {
        if ( xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) != pdTRUE) continue;

        if ( parse_command() == -1) continue;

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
    uint32_t *msg;
    while(1)
    {
        xQueueReceive(print_queue, &msg, portMAX_DELAY);
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen((char *)msg), portMAX_DELAY);
    }
}

// Private Helper functions

static int parse_command(void)
{
    uint8_t item;
    uint8_t i = 0;

    do
    {
        if (xQueueReceive(input_data_queue, &item, portMAX_DELAY) != pdPASS) return -1;
        command.payload[i++] = item;
    } while (item != '\r' && i < 10);

    command.payload[i-1] = '\0';
    command.len = i - 1;

    return 0;
}