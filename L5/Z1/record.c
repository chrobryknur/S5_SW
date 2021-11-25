#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define BTN PD2
#define BTN_PIN PIND

#define LED PD3
#define LED_DDR DDRD

#define BUFFER_SIZE 100
#define BUFFER_BYTES 13 

uint8_t current_bit = 0;
int8_t  vote = 0;
uint8_t count = 0;
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

void io_init(){
    LED_DDR |= _BV(LED);
    PORTD   = _BV(BTN);
}

void timer2_init(){
    TCCR2A = _BV(WGM20);   // CTC mode
    TCCR2B = _BV(CS22);    // prescaler = 64
    TIMSK2 = _BV(TOIE2);   // enable overflow interrupt
    TCNT2  = 131;          // after 124 increments the OVF will be set, giving 1000Hz
    TIFR2  = 0;            // clear interrupt flag register
}

ISR(TIMER2_OVF_vect){
    TCNT2 = 131;           // reset timer, so that after 124 increments OVF is set 
    if(get_bit_value(current_bit)){
        PORTD |= _BV(LED);
    }
    else{
        PORTD &= ~_BV(LED);
    }

    if(BTN_PIN & _BV(BTN)){
        vote--;
    }
    else if(!(BTN_PIN & _BV(BTN))){
        vote++;
    }

    if(++count == 10){
        if(vote > 5){
            set_bit(current_bit);
        }
        else{
            unset_bit(current_bit);
        }
        current_bit = increment_current_bit(current_bit);
        vote = 0;
        count = 0;
    }
}

int main() {
    io_init();
    timer2_init();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();
    while (1) {
        sleep_mode();
    }
}
