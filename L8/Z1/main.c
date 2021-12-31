#include "FreeRTOS.h"
#include "task.h"
#include <avr/io.h>
#include <stdio.h>

#define RECORD_TASK_PRIORITY 1
#define KITT_TASK_PRIORITY 1

#define BTN PB1
#define BTN_PIN PINB

#define LED PB0
#define LED_DDR DDRB

#define BUFFER_SIZE 100
#define BUFFER_BYTES 13 

static inline uint8_t increment_current_bit(uint8_t curr_bit){
    return ++curr_bit == BUFFER_SIZE ? 0 : curr_bit;
}

static inline void set_bit(uint8_t current_bit, uint8_t circular_buffer[]){
    circular_buffer[current_bit >> 3] |= _BV(current_bit & 0b111);
}

static inline void unset_bit(uint8_t current_bit, uint8_t circular_buffer[]){
    circular_buffer[current_bit >> 3] &= ~(_BV(current_bit & 0b111));
}

static inline uint8_t get_bit_value(uint8_t bit, uint8_t circular_buffer[]){
    return circular_buffer[bit >> 3] & _BV(bit & 0b111);
}

static void vRecord(void* pvParameters);
static void vKitt(void* pvParameters);

int main(void)
{
    // Create task.
    xTaskHandle record_handle;
    xTaskHandle kitt_handle;

    xTaskCreate
        (
         vRecord,
         "record",
         configMINIMAL_STACK_SIZE,
         NULL,
         RECORD_TASK_PRIORITY,
         &record_handle
        );

    xTaskCreate
        (
         vKitt,
         "kitt",
         configMINIMAL_STACK_SIZE,
         NULL,
         KITT_TASK_PRIORITY,
         &kitt_handle
        );

    // Start scheduler.
    vTaskStartScheduler();

    return 0;
}

void vApplicationIdleHook(void)
{

}




static void vRecord(void* pvParameters)
{
    const TickType_t delayTime = 10 / portTICK_PERIOD_MS;

    uint8_t circular_buffer[BUFFER_BYTES];
    DDRB |= _BV(LED);
    PORTB = _BV(BTN);

    uint8_t current_bit = 0;

    for ( ;; )
    {
        if(get_bit_value(current_bit, circular_buffer)){
            PORTB |= _BV(LED);
        }
        else{
            PORTB &= ~_BV(LED);
        }

        if(BTN_PIN & _BV(BTN)){
            unset_bit(current_bit, circular_buffer);
        }
        else{
            set_bit(current_bit, circular_buffer);
        }                

        current_bit = increment_current_bit(current_bit);
        vTaskDelay(delayTime);
    }
}

static void vKitt(void* pvParameters)
{
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
    DDRD  = 0xFF;

    const TickType_t delayTime = 100 / portTICK_PERIOD_MS;

    for ( ;; )
    {
        for(int8_t i = PD0; i <= PD6; i++)
        {
            PORTD = (1 << i);
            vTaskDelay(delayTime);
        }
        for(int8_t i = PD7; i >= PD1; i--)
        {
            PORTD = (1 << i);
            vTaskDelay(delayTime);
        }
    }
}
