#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include <util/delay.h>

#define UART_TASK_PRIORITY 1
#define LED_TASK_PRIORITY 2

#define LED PB0
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BLINK_QUEUE_SIZE 10

static QueueHandle_t blink_queue;
static void vUart(void* pvParameters);
static void vLed(void* pvParameters);

extern FILE uart_file;

int main(void)
{
    // Create task.
    xTaskHandle uart_handle;
    xTaskHandle led_handle;

    blink_queue = xQueueCreate(BLINK_QUEUE_SIZE, sizeof(uint32_t));

    xTaskCreate
        (
         vUart,
         "uart",
         configMINIMAL_STACK_SIZE,
         NULL,
         UART_TASK_PRIORITY,
         &uart_handle
        );

    xTaskCreate
        (
         vLed,
         "led",
         configMINIMAL_STACK_SIZE,
         NULL,
         LED_TASK_PRIORITY,
         &led_handle
        );

    // Start scheduler.
    vTaskStartScheduler();

    return 0;
}

void vApplicationIdleHook(void)
{

}

static void vUart(void* pvParameters)
{
    uart_init();
    stdin = stdout = stderr = &uart_file;

    for ( ;; )
    {
        uint32_t blink_time;
        scanf("%"SCNu32, &blink_time);
        putchar('\r');
        putchar('\n');
        xQueueSend(blink_queue, &blink_time, portMAX_DELAY);
    }
}


static void vLed(void* pvParameters)
{
    LED_DDR |= _BV(LED);

    const uint16_t miliseconds_between_blinks = 1000;
    const TickType_t delay_between_blinks = miliseconds_between_blinks / portTICK_PERIOD_MS;

    for ( ;; )
    {
        uint32_t blink_time_ms = 0;
        xQueueReceive(blink_queue, &blink_time_ms, portMAX_DELAY);
        const TickType_t blink_time = blink_time_ms / portTICK_PERIOD_MS;

        LED_PORT |= _BV(LED);
        vTaskDelay(blink_time);

        LED_PORT &= ~_BV(LED);
        vTaskDelay(delay_between_blinks);
    }
}
