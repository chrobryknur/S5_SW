#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <stdlib.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)

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

void print_int8_t()
{
    printf("Enter two int8_t numbers: \r\n");
    int8_t a_8, b_8;
    scanf("%"SCNd8" %"SCNd8, &a_8, &b_8);
    printf("int8_t: %"PRId8" %"PRId8" %"PRId8"\r\n", a_8+b_8, a_8*b_8, a_8/b_8);
}

void print_int16_t()
{
    printf("Enter two int16_t numbers: \r\n");
    int16_t a_16, b_16;
    scanf("%"SCNd16" %"SCNd16, &a_16, &b_16);
    printf("int16_t: %"PRId16" %"PRId16" %"PRId16"\r\n", a_16+b_16, a_16*b_16, a_16/b_16);
}

void print_int32_t()
{
    printf("Enter two int32_t numbers: \r\n");
    int32_t a_32, b_32;
    scanf("%"SCNd32" %"SCNd32, &a_32, &b_32);
    printf("int32_t: %"PRId32" %"PRId32" %"PRId32"\r\n", a_32+b_32, a_32*b_32, a_32/b_32);
}

void print_int64_t()
{
    printf("Enter two int64_t number: \r\n");
    int64_t a_64, b_64;
    scanf("%"SCNd32" %"SCNd32, (int32_t*)&a_64, (int32_t*)&b_64);
    printf("int64_t: %"PRId32" %"PRId32" %"PRId32"\r\n", (int32_t)(a_64+b_64), (int32_t)(a_64*b_64), (int32_t)(a_64/b_64));
}

void print_float()
{
    printf("Enter two float numbers: \r\n");
    float a_f, b_f;
    scanf("%f %f", &a_f, &b_f);
    printf("float: %f %f %f \r\n", a_f+b_f, a_f*b_f, a_f/b_f);
}

int main()
{
    uart_init();

    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;

    while(1){
        print_int8_t();
        print_int16_t();
        print_int32_t();
        print_int64_t();
        print_float();
    }
}

