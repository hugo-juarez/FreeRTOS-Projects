//
// Task Handlers
//

#include "task_handler.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "led_effect.h"
#include "rtc.h"

// Private Helper functions defines
static int parse_command(void);
static void show_time_date(void);

// Private global variables
static State_t curr_state = State_MainMenu;
static Command_t command;
static const char* invalid_option = "////Invalid option////\n";
static UART_HandleTypeDef *huart;
static char time_buffer[40];
static char date_buffer[40];

// RTC Cases private functions
static void rtc_menu_handle(void);
static int rtc_time_config_handle(int is_hour);
static int rtc_date_config_handle(int date_type);
static void rtc_report_handle(void);

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

// Print Task UART passing
int set_huart_print(UART_HandleTypeDef *p_huart)
{
    if (p_huart == NULL) return 0;
    huart = p_huart;
    return 1;
}

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
    const char* msg_rtc1 = "========================\n"
                            "|         RTC          |\n"
                            "========================\n";

    const char* msg_rtc2 = "Configure Time            ----> 0\n"
                            "Configure Date            ----> 1\n"
                            "Enable reporting          ----> 2\n"
                            "Exit                      ----> 4\n"
                            "Enter your choice here : ";


    const char *msg_rtc_hh = "Enter hour(1-12):";
    const char *msg_rtc_mm = "Enter minutes(0-59):";
    const char *msg_rtc_ss = "Enter seconds(0-59):";

    const char *msg_rtc_dd  = "Enter date(1-31):";
    const char *msg_rtc_mo  ="Enter month(1-12):";
    const char *msg_rtc_dow  = "Enter day(1-7 sun:1):";
    const char *msg_rtc_yr  = "Enter year(0-99):";

    const char *msg_conf = "Configuration successful\n";
    const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";

    while(1)
    {
        xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

        xQueueSendToBack(print_queue, &msg_rtc1, portMAX_DELAY);
        show_time_date();

        while (curr_state != State_MainMenu)
        {
            // Managing input
            switch (curr_state)
            {
            case State_RtcMenu:
                xQueueSendToBack(print_queue, &msg_rtc2, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                rtc_menu_handle();
                break;
            case State_RtcTimeConfig:
                int value = 0;
                RTC_TimeTypeDef time = {0};

                // Getting hour
                xQueueSendToBack(print_queue, &msg_rtc_hh, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                value = rtc_time_config_handle(1);
                if (value == -1) break;
                time.Hours = value;

                // Getting minutes
                xQueueSendToBack(print_queue, &msg_rtc_mm, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                value = rtc_time_config_handle(0);
                if (value == -1) break;
                time.Minutes = value;

                // Getting seconds
                xQueueSendToBack(print_queue, &msg_rtc_ss, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                value = rtc_time_config_handle(0);
                if (value == -1) break;
                time.Seconds = value;

                time.TimeFormat = RTC_HOURFORMAT12_AM;

                rtc_config_time(&time);
                curr_state = State_MainMenu;
                break;
            case State_RtcDateConfig:
                int date_value = 0;
                RTC_DateTypeDef date = {0};

                xQueueSendToBack(print_queue, &msg_rtc_dd, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                date_value = rtc_date_config_handle(0);
                if (date_value == -1) break;
                date.Date = date_value;

                xQueueSendToBack(print_queue, &msg_rtc_mo, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                date_value = rtc_date_config_handle(1);
                if (date_value == -1) break;
                date.Month = date_value;

                xQueueSendToBack(print_queue, &msg_rtc_dow, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                date_value = rtc_date_config_handle(2);
                if (date_value == -1) break;
                date.WeekDay = date_value;

                xQueueSendToBack(print_queue, &msg_rtc_yr, portMAX_DELAY);
                xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
                date_value = rtc_date_config_handle(3);
                if (date_value == -1) break;
                date.Year = date_value;

                rtc_config_date(&date);

                curr_state = State_MainMenu;

                break;
            case State_RtcReport:
                break;
            default:
                break;
            }
        }

        xTaskNotify(menu_task, 0, eNoAction);
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
        HAL_UART_Transmit(huart, (uint8_t *)msg, strlen((char *)msg), portMAX_DELAY);
    }
}

// RTC State Hanlders
static void rtc_menu_handle(void)
{
    if (command.len > 1)
    {
        xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
        curr_state = State_MainMenu;
        return;
    }

    const uint8_t option = command.payload[0] - 48;

    switch (option)
    {
    case 0:
        curr_state = State_RtcTimeConfig;
        break;
    case 1:
        curr_state = State_RtcDateConfig;
        break;
    case 2:
        curr_state = State_RtcReport;
        break;
    default:
        xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
        curr_state = State_MainMenu;
        break;
    }
}
static int rtc_time_config_handle(int is_hour)
{
    if (command.len > 2)
    {
        xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
        curr_state = State_MainMenu;
        return -1;
    }

    // Convert to number
    int val = atoi((char *)command.payload);

    int hour_check = is_hour && (val < 1 || val > 12);
    int min_sec_check = val < 0 || val > 59;

    if ( hour_check || min_sec_check )
    {
        xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
        curr_state = State_MainMenu;
        return -1;
    }

    return val;

}
static int rtc_date_config_handle(int date_type)
{
    /* Date type is
     * 0: day
     * 1: month
     * 2: day of week
     * 3: year
     */

     if( command.len > 2 || (date_type == 2 && command.len > 1))
     {
         xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
         curr_state = State_MainMenu;
         return -1;
     }

    // Value
    int val = atoi((char *)command.payload);

    // Checks for day/month/day of week/year
    const int day_check = date_type == 0 && ( val < 1 || val > 31);
    const int month_check = date_type == 1 && ( val < 1 || val > 12);
    const int dow_check = date_type == 2 && ( val < 1 || val > 7);
    const int year_check = date_type == 3 && ( val < 0 || val > 99);

    if (day_check || month_check || dow_check || year_check)
    {
        xQueueSendToBack(print_queue, &invalid_option, portMAX_DELAY);
        curr_state = State_MainMenu;
        return -1;
    }

    return val;

}
static void rtc_report_handle(void)
{

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

static void show_time_date(void)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    rtc_get_time(&time);
    rtc_get_date(&date);

    char *format = (time.TimeFormat == RTC_HOURFORMAT12_AM) ? "AM" : "PM";

    // Helper pointers to pass a pointer of another pointer instead of a pointer to a value.
    const char* showtime = time_buffer;
    const char* showdate = date_buffer;

    /* Display time Format : hh:mm:ss [AM/PM] */
    sprintf((char*)time_buffer,"%s:\t%02d:%02d:%02d [%s]","\nCurrent Time&Date",time.Hours, time.Minutes, time.Seconds,format);
    xQueueSendToBack(print_queue,&showtime,portMAX_DELAY);

    /* Display date Format : date-month-year */
    sprintf((char*)date_buffer,"\t%02d-%02d-%2d\n",date.Month, date.Date, 2000 + date.Year);
    xQueueSendToBack(print_queue,&showdate,portMAX_DELAY);
}