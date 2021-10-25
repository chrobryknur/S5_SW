#include <avr/io.h>
#include <util/delay.h>

#define BTN PD2
#define BTN_PIN PIND

#define LED PD3
#define LED_DDR DDRD

#define BUFFER_SIZE 100

int16_t increment_current_bit(int16_t curr_bit){
    if(curr_bit+1 == BUFFER_SIZE)
        return 0;
    return curr_bit+1;
}

int16_t decrement_current_bit(int16_t curr_bit){
    if(curr_bit-1 == -1)
        return BUFFER_SIZE-1;
    return curr_bit-1;
}


int main() {
    LED_DDR |= _BV(LED);
    PORTD   |= _BV(BTN);

    int16_t is_recording_in_progress = 0;
    int16_t current_bit = 0;
    int16_t starting_bit;
    int16_t current_period = 0;
    int16_t circular_buffer[BUFFER_SIZE];

    while (1) {
        if(BTN_PIN & _BV(BTN)){

            circular_buffer[current_bit] = 1;
            PORTD &= ~(_BV(LED));
        }
        else{
            circular_buffer[current_bit] = 0;
            PORTD |= _BV(LED);
            if(!is_recording_in_progress){
                is_recording_in_progress = 1;
                starting_bit = current_bit;
                current_period = 0;
            }
        }                
        current_bit = increment_current_bit(current_bit);
        _delay_ms(10);

        if(is_recording_in_progress && ((++current_period)== BUFFER_SIZE)){
            is_recording_in_progress = 0;

            int16_t counter = 0;

            for(int16_t i = starting_bit; counter < BUFFER_SIZE-1; i = increment_current_bit(i)){
                if(circular_buffer[i]){
                    PORTD &= ~(_BV(LED));
                }
                _delay_ms(10);
                PORTD |= _BV(LED);
                counter++;
            }
        }
    }
}
