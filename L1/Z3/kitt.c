#include <avr/io.h>
#include <util/delay.h>

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
        for(int8_t i = PD0; i <= PD6; i++)
        {
            PORTD = (1 << i);
            _delay_ms(100);
        }
        for(int8_t i = PD7; i >= PD1; i--)
        {
            PORTD = (1 << i);
            _delay_ms(100);
        }
    }
}
