#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define LA PB1
#define OE PB2

volatile uint8_t current_digit = 0;

static uint8_t digit_bits[] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111,
};

void spi_init(){
    DDRB |= _BV(DDB3) | _BV(DDB5) | _BV(DDB2) | _BV(LA) | _BV(OE);
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

void spi_transfer(uint8_t data){
    cli();
    PORTB |= _BV(OE); // turn LEDs OFF
    /* PORTB |= _BV(LA); // start transmission */

    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    SPSR |= _BV(SPIF);
    
    /* PORTB &= ~_BV(LA); // end transmission */
    PORTB &= ~_BV(OE); // turn LEDs ON
    sei();
}

void timer1_init()
{
  // ustaw tryb licznika
  // COM1A = 10   -- non-inverting mode
  // WGM1  = 1110 -- fast PWM top=ICR1
  // CS1   = 101  -- prescaler 1024
  // ICR1  = 15624
  // częstotliwość 16e6/(1024*(1+15624)) = 1 Hz
  // wzór: datasheet 20.12.3 str. 164
  ICR1 = 15624;
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS10) | _BV(CS12);
  TIMSK1 = _BV(TOIE1); // TOV1 flag
}

ISR(TIMER1_OVF_vect){
    spi_transfer(digit_bits[current_digit]);
    if(++current_digit == 10)
        current_digit = 0;
}

int main(){
    spi_init();
    timer1_init();

    PORTB |= _BV(OE); // turn leds OFF

    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while(1) {
        sleep_mode();
    }
}

