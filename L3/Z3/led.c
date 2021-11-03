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

void delay(uint16_t us){
    while(us--) _delay_us(1);
}
int main(){
    initADC0();
    turn_tx_and_rx_off();
    DDRD = 0xFF;

    uint16_t adcValue;

    while(1){
        ADCSRA |= _BV(ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        adcValue = ADC;

        PORTD = _BV(PD2);
        delay(1 << (adcValue/100));
        /* switch(adcValue / 100){ */
        /*     case 0: */
        /*         delay(1 << 0); */
        /*         break; */
        /*     case 1: */
        /*         delay(1 << 1); */
        /*         break; */
        /*     case 2: */
        /*         delay(1 << 2); */
        /*         break; */
        /*     case 3: */
        /*         delay(1 << 3); */
        /*         break; */
        /*     case 4: */
        /*         delay(1 << 4); */
        /*         break; */
        /*     case 5: */
        /*         delay(1 << 5); */
        /*         break; */
        /*     case 6: */
        /*         delay(1 << 6); */
        /*         break; */
        /*     case 7: */
        /*         delay(1 << 7); */
        /*         break; */
        /*     case 8: */
        /*         delay(1 << 8); */
        /*         break; */
        /*     case 9: */
        /*         delay(1 << 9); */
        /*         break; */
        /*     case 10: */
        /*         delay(1 << 10); */
        /*         break; */
        /*     case 11: */
        /*         delay(1 << 11); */
        /*         break; */
        /*     case 12: */
        /*         delay(1 << 12); */
        /*         break; */
        /*     default: */
        /*         delay(1 << 13); */
        /*         break; */
        /* } */
        PORTD = 0;
        delay(1 << (10 - adcValue/100));
        /* switch(adcValue/100){ */
        /*     case 0: */
        /*         delay(1 << 10); */
        /*         break; */
        /*     case 1: */
        /*         delay(1 << 9); */
        /*         break; */
        /*     case 2: */
        /*         delay(1 << 8); */
        /*         break; */
        /*     case 3: */
        /*         delay(1 << 7); */
        /*         break; */
        /*     case 4: */
        /*         delay(1 << 6); */
        /*         break; */
        /*     case 5: */
        /*         delay(1 << 5); */
        /*         break; */
        /*     case 6: */
        /*         delay(1 << 4); */
        /*         break; */
        /*     case 7: */
        /*         delay(1 << 3); */
        /*         break; */
        /*     case 8: */
        /*         delay(1 << 2); */
        /*         break; */
        /*     case 9: */
        /*         delay(1 << 1); */
        /*         break; */
        /*     default: */
        /*         break; */
        /* } */
    }
}
