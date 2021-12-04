#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

void uart_init(){
  UBRR0 = UBRR_VALUE;
  UCSR0A = 0;
  UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

ISR(USART_RX_vect){
    char data = UDR0;
    if(data == '\r'){
        UDR0 = '\r';
        UDR0 = '\n';
    }
    else{
        UDR0 = data;
    }
}

int main(){
    uart_init();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while(1){
        sleep_mode();
    }
}

