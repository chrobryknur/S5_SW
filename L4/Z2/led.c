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

void timer1_init()
{
  // ustaw tryb licznika
  // COM1A = 10   -- non-inverting mode
  // WGM1  = 1110 -- fast PWM top=ICR1
  // CS1   = 010 -- prescaler 8
  // ICR1  = 999
  // częstotliwość 16e6/(8*(1+99)) = 2kHz
  // wzór: datasheet 20.12.3 str. 164
  ICR1 = 999;
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS11);
  // ustaw pin OC1A (PB1) jako wyjście
  DDRB |= _BV(PB1);
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
    timer1_init();

    uint16_t adcValue;

    while(1){
        ADCSRA |= _BV(ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        adcValue = ADC >> 4; // 0 <= ADC <= 500, 0 <= adcValue <= 32 
        printf("%"PRIu16 " %"PRIu16"\n\r", ADC,  adcValue);
        OCR1A = ICR1 / adcValue;
    }
}
