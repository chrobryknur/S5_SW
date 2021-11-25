#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

volatile int8_t  a8  = 10, b8  = 10;
volatile int16_t a16 = 1023, b16 = 1025;
volatile int32_t a32 = 1032132, b32 = 0;
volatile int64_t a64 = 103213321, b64 = 10321321;
volatile float   af  = 10.1, bf  = 10.2;

void measure_int8_t(){
    uint16_t start, end;
    volatile int8_t add, sub, mul, div;

    start = TCNT1;
    add = a8 + b8;
    end = TCNT1;

    printf("int8_t addition: %"PRIu8" %"PRIu16"\n\r", add, end - start);

    start = TCNT1;
    sub = a8 - b8;
    end = TCNT1;
    printf("int8_t substraction: %"PRIu8" %"PRIu16"\n\r", sub, end - start);

    start = TCNT1;
    mul = a8 * b8;
    end = TCNT1;
    printf("int8_t multiplication: %"PRIu8" %"PRIu16"\n\r", mul, end - start);
    
    start = TCNT1;
    div = a8 / b8;
    end = TCNT1;
    printf("int8_t division: %"PRIu8" %"PRIu16"\n\r", div, end - start);
}

void measure_int16_t(){
    uint16_t start, end;
    volatile int16_t add, sub, mul, div;

    start = TCNT1;
    add = a16 + b16;
    end = TCNT1;

    printf("int16_t addition: %"PRIu16" %"PRIu16"\n\r", add, end - start);

    start = TCNT1;
    sub = a16 - b16;
    end = TCNT1;
    printf("int16_t substraction: %"PRIu16" %"PRIu16"\n\r", sub, end - start);

    start = TCNT1;
    mul = a16 * b16;
    end = TCNT1;
    printf("int16_t multiplication: %"PRIu16" %"PRIu16"\n\r", mul, end - start);
    
    start = TCNT1;
    div = a16 / b16;
    end = TCNT1;
    printf("int16_t division: %"PRIu16" %"PRIu16"\n\r", div, end - start);
}

void measure_int32_t(){
    uint16_t start, end;
    volatile int32_t add, sub, mul, div;

    start = TCNT1;
    add = a32 + b32;
    end = TCNT1;

    printf("int32_t addition: %"PRIu32" %"PRIu16"\n\r", add, end - start);

    start = TCNT1;
    sub = a32 - b32;
    end = TCNT1;
    printf("int32_t substraction: %"PRIu32" %"PRIu16"\n\r", sub, end - start);

    start = TCNT1;
    mul = a32 * b32;
    end = TCNT1;
    printf("int32_t multiplication: %"PRIu32" %"PRIu16"\n\r", mul, end - start);
    
    start = TCNT1;
    div = a32 / b32;
    end = TCNT1;
    printf("int32_t division: %"PRIu32" %"PRIu16"\n\r", div, end - start);
}


void measure_int64_t(){
    uint16_t start, end;
    volatile int64_t add, sub, mul, div;

    start = TCNT1;
    add = a64 + b64;
    end = TCNT1;

    printf("int64_t addition: %"PRIu32" %"PRIu16"\n\r", (int32_t)add, end - start);

    start = TCNT1;
    sub = a64 - b64;
    end = TCNT1;
    printf("int64_t substraction: %"PRIu32" %"PRIu16"\n\r", (int32_t)sub, end - start);

    start = TCNT1;
    mul = a64 * b64;
    end = TCNT1;
    printf("int64_t multiplication: %"PRIu32" %"PRIu16"\n\r", (int32_t)mul, end - start);
    
    start = TCNT1;
    div = a64 / b64;
    end = TCNT1;
    printf("int64_t division: %"PRIu32" %"PRIu16"\n\r", (int32_t)div, end - start);
}

void measure_float(){
    uint16_t start, end;
    volatile float add, sub, mul, div;

    start = TCNT1;
    add = af + bf;
    end = TCNT1;

    printf("float addition: %f %"PRIu16"\n\r", add, end - start);

    start = TCNT1;
    sub = af - bf;
    end = TCNT1;
    printf("float substraction: %f %"PRIu16"\n\r", sub, end - start);

    start = TCNT1;
    mul = af * bf;
    end = TCNT1;
    printf("float multiplication: %f %"PRIu16"\n\r", mul, end - start);
    
    start = TCNT1;
    div = af / bf;
    end = TCNT1;
    printf("float division: %f %"PRIu16"\n\r", div, end - start);
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

void timer1_init()
{
  // ustaw tryb licznika
  // WGM1  = 0000 -- normal
  // CS1   = 001  -- prescaler 1
  TCCR1B = _BV(CS10);
}
FILE uart_file;

int main()
{
  uart_init();
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  timer1_init();
  while(1) {
    measure_int8_t();
    measure_int16_t();
    measure_int32_t();
    measure_int64_t();
    measure_float();
    _delay_ms(10000);
  }
}
