#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define TIMER1_FREQUENCY 2000000

void uart_init()
{
  UBRR0 = UBRR_VALUE;
  UCSR0A = 0;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_transmit(char data, FILE *stream)
{
  while(!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

int uart_receive(FILE *stream)
{
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

FILE uart_file;

volatile uint16_t prev_timestamp, curr_timestamp, cycles_between_timestamps;
volatile uint32_t frequency;

void timer1_init(){
    TCCR1B |= _BV(ICNC1); // enable noise canceler
    TCCR1B |= _BV(CS11);  // prescaler - 8 - TIMER1 freq = 2MHz = CPU_FREQUENCY
    TCCR1B |= _BV(ICES1); // input capture on rising edge
    TIMSK1 |= _BV(ICIE1); // enable input capture interrupt
}

ISR(TIMER1_CAPT_vect){
    curr_timestamp = ICR1;
    cycles_between_timestamps = curr_timestamp - prev_timestamp;
    frequency = TIMER1_FREQUENCY / cycles_between_timestamps;
    prev_timestamp = curr_timestamp;
}

int main() {
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    timer1_init();
    sei();
    set_sleep_mode(SLEEP_MODE_IDLE);
    while (1) {
        sleep_mode();
        printf("%"PRId32"Hz\r\n", frequency);
    }
}
