#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define BAUD 9600 
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define BTN_RESET PC2
#define BTN_PREV PC1
#define BTN_NEXT PC0
#define BTN_PIN PINC
#define BTN_PORT PORTC

#define LED_DDR DDRD
#define CYCLE_DELAY 200

#define ANTI_DEBOUNCING_READS 5
#define ANTI_DEBOUNCING_DELAY 1

uint8_t binary_to_gray(uint8_t number){
    return number ^ (number >> 1);
}

void turn_tx_and_rx_off() {
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
}

uint8_t is_btn_pushed(uint8_t BTN){
    uint8_t flag = 0xFF;
    for(uint8_t i=0; i<ANTI_DEBOUNCING_READS; i++){
        flag &= (BTN_PIN & _BV(BTN));
        _delay_ms(ANTI_DEBOUNCING_DELAY);
    }
    return flag;
}

int main() {
    turn_tx_and_rx_off();
    LED_DDR  = 0xFF;
    BTN_PORT = _BV(BTN_RESET) | _BV(BTN_PREV) | _BV(BTN_NEXT);

    uint8_t gray_code = 0;
    PORTD=0;

    while (1) {
        if(is_btn_pushed(BTN_RESET)){
            gray_code = 0;
            PORTD = binary_to_gray(gray_code);
            while(is_btn_pushed(BTN_RESET));
        }

        if(is_btn_pushed(BTN_PREV)){
            PORTD = binary_to_gray(--gray_code);
            while(is_btn_pushed(BTN_PREV));
        }

        if(is_btn_pushed(BTN_NEXT)){
            PORTD = binary_to_gray(++gray_code);
            while(is_btn_pushed(BTN_NEXT));
        }
        _delay_ms(CYCLE_DELAY);
    }
}
