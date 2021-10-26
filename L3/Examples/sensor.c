#include <avr/io.h>
#include <util/delay.h>

static inline void initADC0(){
    ADMUX  |= _BV(REFS0);
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) /*| _BV(ADPS0)*/;
    ADCSRA |= _BV(ADEN);
}

static inline void turn_tx_and_rx_off() {
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
}

int main(){
    initADC0();
    turn_tx_and_rx_off();
    DDRD = 0xFF;

    uint8_t ledValue;
    uint16_t adcValue;

    while(1){
        ADCSRA |= _BV(ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        adcValue = ADC;

        ledValue = adcValue >> 7;
        PORTD = 0;
        for(uint8_t i = 0; i <= ledValue; i++){
            PORTD |= _BV(i);
        }
        _delay_ms(50);
    }
}
