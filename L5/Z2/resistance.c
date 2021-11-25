#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

static inline void initADC0(){
    ADMUX  |= _BV(REFS0); // V_CC
    ADCSRA |= _BV(ADEN);
    ADCSRA |= _BV(ADIE);  // enable ADC Interrupt
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // set prescaler to 128
}

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

volatile uint8_t number_of_interrupts = 0;
volatile float resistance = 0.0;

void io_init()
{
  // ustaw pull-up na PD2 (INT0)
  PORTD |= _BV(PORTD2);
  // ustaw wyzwalanie przerwania na INT0 zboczem narastającym
  EICRA |= _BV(ISC00) | _BV(ISC01);
  // odmaskuj przerwanie dla INT0
  EIMSK |= _BV(INT0);
}

void timer2_init(){
    TCCR2B =  _BV(CS20) | _BV(CS21) | _BV(CS22); // normal mode, prescaler = 1024
    TIMSK2 = _BV(TOIE2);                         // enable overflow flag
}

const float V_in  = 5.0;
const float R2    = 10000.0;
volatile float V_out;

ISR(ADC_vect){
    V_out = (ADC * V_in) / 1024.0;

    resistance = (R2 * (V_in - V_out)) / V_out;
    ADCSRA &= ~_BV(ADEN); // turn ADC off
}

ISR(INT0_vect){
    ADCSRA |= _BV(ADEN);  // turn ADC on
}

ISR(TIMER2_OVF_vect){
    number_of_interrupts++;
}

int main() {
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    initADC0();
    io_init();
    timer2_init();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();
    while (1) {
        sleep_mode();
        if(number_of_interrupts > 10){
            printf("Resistance: %f\r\n", resistance);
            number_of_interrupts = 0;
        }
    }
}
