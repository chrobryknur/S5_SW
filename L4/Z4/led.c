#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void turn_tx_and_rx_off() {
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
}

void set_portd_to_output_mode() {
    DDRD  = _BV(PD0) | _BV(PD1);
    PORTD = _BV(PD0) | _BV(PD1);
}

void timer1_init()
{
    //prescaler = 1 - CS10
    //
    TCCR1A = _BV(COM1A0);
    TCCR1B = _BV(WGM12) | _BV(CS10);
    OCR1A = 210;
}

void delay(uint16_t ms){
    while(ms--) _delay_us(1);
}

int main(){
    turn_tx_and_rx_off();
    set_portd_to_output_mode();
    timer1_init();

    while(1){
        DDRB = _BV(PB1);
        for(uint16_t i = 0; i < 600; i++){
            if(PORTB & _BV(PB0)){
                PORTD = ~_BV(PD0);
            }
            delay(1);
        }
        DDRB = 0;
        delay(600);
    }
}
