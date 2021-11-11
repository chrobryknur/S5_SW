#include <avr/io.h>
#include <util/delay.h>

static uint8_t digit_bits[] = {
        ((1<<PD6)  | (1<<PD7)),                         //0
        ~((1<<PD1) | (1<<PD2)),                         //1
        ((1<<PD2)  | (1<<PD5) | (1<<PD7)),              //2
        ((1<<PD4)  | (1<<PD5) | (1<<PD7)),              //3
        ~((1<<PD1) | (1<<PD2) | (1<<PD5) | (1<<PD6)),   //4
        ((1<<PD1)  | (1<<PD4) | (1<<PD7)),              //5
        ((1<<PD1)  | (1<<PD7)),                         //6
        ~((1<<PD0) | (1<<PD1) | (1<<PD2)),              //7
        (1<<PD7),                                       //8
        ((1<<PD7) | (1<<PD4))                           //9
};

void turn_tx_and_rx_off() {
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
}

void set_portd_to_output_mode() {
    DDRD  = 0xFF;
}

int main() {
    turn_tx_and_rx_off();
    set_portd_to_output_mode();
    while (1) {
        for(int8_t i = 0; i <= 9; i++)
        {
            PORTD = digit_bits[i];
            _delay_ms(1000);
        }
        PORTD = 0xFF;
        _delay_ms(1000);
    }
}
