#include <avr/io.h>
#include <util/delay.h>

static inline void initADC0(){
    ADMUX  |= _BV(REFS0);
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRA |= _BV(ADEN);
}

static inline void turn_tx_and_rx_off() {
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
}

static inline void delay(uint16_t us){
    while(us--) _delay_us(1);
}

static inline void init_led(){
    DDRD = 0xFF;
    PORTD = 0;
}

int main(){
    initADC0();
    turn_tx_and_rx_off();
    init_led();

    uint16_t adcValue;

    while(1){
        ADCSRA |= _BV(ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        adcValue = ADC >> 7;

        PORTD = _BV(PD2);
        delay(1 << (adcValue));

        PORTD = 0;
        delay(1 << (10 - adcValue));
    }
}
