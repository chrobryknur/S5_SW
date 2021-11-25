#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

#define NUMBER_OF_MEASUREMENTS 20

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

static inline void initADC0(){
    ADMUX  |= _BV(REFS0); // V_CC
    ADMUX  |= _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // select V_BG (1.1V) as the input channel
    ADCSRA |= _BV(ADEN);
    ADCSRA |= _BV(ADPS2) /*| _BV(ADPS1) | _BV(ADPS0)*/; // set prescaler to 128
}

float calculate_variance(uint16_t* arr){
    float avg = 0;
    for(uint8_t i = 0; i < NUMBER_OF_MEASUREMENTS; i++){
        avg += arr[i];
    }

    avg /= NUMBER_OF_MEASUREMENTS;

    float variance = 0;

    for(uint8_t i = 0; i< NUMBER_OF_MEASUREMENTS; i++){
        variance += (arr[i] - avg) * (arr[i] - avg);
    }

    variance /= NUMBER_OF_MEASUREMENTS;
    return variance;
}

uint16_t normal[NUMBER_OF_MEASUREMENTS];
uint8_t normal_iter = 0;

uint16_t noise_reduction[NUMBER_OF_MEASUREMENTS];
uint8_t noise_reduction_iter = 0;

ISR(ADC_vect){
    uint16_t temp = ADC;
    noise_reduction[noise_reduction_iter++] = temp;
    printf("R: %"PRIu16" ", temp);
    _delay_ms(10);
}

int main() {
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    initADC0();
    /* set_sleep_mode(SLEEP_MODE_IDLE); */
    SMCR = _BV(SM0); // ADC Noise Reduction sleep mode
    sei();
    while (1) {
        /* sleep_mode(); */
        if(normal_iter < NUMBER_OF_MEASUREMENTS){
            ADCSRA |= _BV(ADSC);
            loop_until_bit_is_clear(ADCSRA, ADSC);
            uint16_t temp = ADC;
            normal[normal_iter++] = temp;
            if(normal_iter == NUMBER_OF_MEASUREMENTS){
                putchar('\r');
                putchar('\n');
            }
            printf("%"PRIu16" ", temp);
        }
        else if(noise_reduction_iter < NUMBER_OF_MEASUREMENTS){
            ADCSRA |= _BV(ADIE);  // enable ADC Interrupt
            ADCSRA |= _BV(ADSC);
            sleep_mode();
            ADCSRA &= ~_BV(ADSC);
            if(noise_reduction_iter == NUMBER_OF_MEASUREMENTS){
                putchar('\r');
                putchar('\n');
            }
        }
        else{
            normal_iter = 0;
            noise_reduction_iter = 0;
            ADCSRA &= ~_BV(ADIE);

            _delay_ms(1000);
        }

    }
}
