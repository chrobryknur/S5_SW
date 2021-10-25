#include <avr/io.h>
#include <util/delay.h>

static uint8_t digit_bits[] = {
        ((1<<PD6)  | (1<<PD7)),                                             //0
        ~((1<<PD1) | (1<<PD2)),                                             //1
        ((1<<PD2)  | (1<<PD5) | (1<<PD7)),                                  //2
        ((1<<PD4)  | (1<<PD5) | (1<<PD7)),                                  //3
        ~((1<<PD1) | (1<<PD2) | (1<<PD5) | (1<<PD6)),                       //4
        ((1<<PD1)  | (1<<PD4) | (1<<PD7)),                                  //5
        ((1<<PD1)  | (1<<PD7)),                                             //6
        ~((1<<PD0) | (1<<PD1) | (1<<PD2)),                                  //7
        (1<<PD7),                                                           //8
        ((1<<PD7) | (1<<PD4))                                               //9
};

void turn_tx_and_rx_off() {
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
}

int main() {
    turn_tx_and_rx_off();
    DDRD = 0xFF;
    DDRC = _BV(PC1) | _BV(PC0);
    while (1) {
        for(int8_t i = 0; i <= 59; i++)
        {
            for(int8_t j = 0; j<100; j++){
                PORTC = _BV(PC0);
                PORTD = digit_bits[i%10];
                _delay_ms(5);
                PORTC = _BV(PC1);
                PORTD = digit_bits[i/10];
                _delay_ms(5);
            }
        }
    }
}
