#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

static inline void initADC0(){
    ADMUX  |= _BV(REFS0);
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRA |= _BV(ADEN);
}

void turn_tx_and_rx_off() {
    UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
}

void set_portd_to_output_mode() {
    DDRD  = _BV(PD0) | _BV(PD1);
    PORTD = _BV(PD0) | _BV(PD1);
}

void timer1_init()
{
    //prescaler = 1 - CS10
    //
    TCCR1A = _BV(COM1A0);
    TCCR1B = _BV(WGM12) | _BV(CS10);
    OCR1A = 209;
}

void timer0_init(){
    TCCR0A = _BV(COM0A0);
    TCCR0B = _BV(WGM01) | _BV(CS02);
    OCR0A = 74;
}

void delay(uint16_t ms){
    while(ms--) _delay_us(1);
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

int main(){
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    initADC0();
    timer0_init();
    timer1_init();

    DDRD |= _BV(PD6); // set OC0A as output;

    while(1){
        if(PORTD & _BV(PD6)){
            printf("impulse\n\r");
            DDRB = _BV(PB1);
        }
        else{
            printf("no impulse\n\r");
            DDRB = 0;
        }
        if(PORTB & _BV(PB0)){
            printf("detected\n\r");
            PORTD = _BV(PD0);
        }
        else{
            printf("not detected\n\r");
            PORTD = 0; 
        }
    }
}

