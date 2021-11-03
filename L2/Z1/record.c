#include <avr/io.h>
#include <util/delay.h>

#define BTN PD2
#define BTN_PIN PIND

#define LED PD3
#define LED_DDR DDRD

#define BUFFER_SIZE 100
#define BUFFER_BYTES 13 

uint8_t circular_buffer[BUFFER_BYTES];

inline uint8_t increment_current_bit(uint8_t curr_bit){
    return ++curr_bit == BUFFER_SIZE ? 0 : curr_bit;
}

inline void set_bit(uint8_t current_bit){
    circular_buffer[current_bit >> 3] |= _BV(current_bit & 0b111);
}

inline void unset_bit(uint8_t current_bit){
    circular_buffer[current_bit >> 3] &= ~(_BV(current_bit & 0b111));
}

inline uint8_t get_bit_value(uint8_t bit){
    return circular_buffer[bit >> 3] & _BV(bit & 0b111);
}


int main() {
    LED_DDR |= _BV(LED);
    PORTD   = _BV(BTN);

    uint8_t current_bit = 0;

    while (1) {
        if(get_bit_value(current_bit)){
            PORTD |= _BV(LED);
        }
        else{
            PORTD &= ~_BV(LED);
        }

        if(BTN_PIN & _BV(BTN)){
            unset_bit(current_bit);
        }
        else{
            set_bit(current_bit);
        }                

        current_bit = increment_current_bit(current_bit);

        _delay_ms(10);
    }
}
