#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include "i2c.h"
#include <string.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

void uart_init(){
  UBRR0 = UBRR_VALUE;
  UCSR0A = 0;
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_transmit(char data, FILE *stream){
  while(!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

int uart_receive(FILE *stream){
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

FILE uart_file;

const uint8_t eeprom_addr = 0xa0;

uint8_t read_byte(uint16_t addr){
    i2cStart();
    i2cSend(eeprom_addr | ((addr & 0x100) >> 7)); // Select slave device and chip
    i2cSend(addr & 0xff);                         // Send address to be read from the selected chip
    i2cStart();
    i2cSend(eeprom_addr | 0x1 | ((addr & 0x100) >> 7)); // Send read request
    uint8_t data = i2cReadNoAck();                // Read data
    i2cStop();
    return data;
}

void write_byte(uint16_t addr, uint8_t value){
    i2cStart();
    i2cSend(eeprom_addr | ((addr & 0x100) >> 7)); // Select slave device and chip
    i2cSend(addr & 0xff);                         // Send address to write to
    i2cSend(value);                               // Send value to be written to address
    i2cStop();
}

int main(){
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    i2cInit();

    uint16_t addr = 0;
    uint8_t value = 0;

    const char s[2] = " ";
    char *token;

    while(1) {
        char c;
        char buffer[32] = {'\0'};
        uint8_t idx = 0;
        while((c = getchar()) != '\r'){
            buffer[idx++] = c;
            putchar(c);
        }

        putchar('\r');
        putchar('\n');

        token = strtok(buffer, s);

        if(!strcmp(token, "read")){
            token = strtok(NULL, s);
            if(token){
                addr = atoi(token);
                printf("read (0x%"PRIx16"): %"PRId8"\r\n", addr, read_byte(addr));
            }
            else{
                printf("No address provided to read!\r\n");
            }
        }
        else if(!strcmp(token, "write")){
            token = strtok(NULL, s);
            if(token){
                addr  = atoi(token);
                token = strtok(NULL, s);
                if(token){
                    value = atoi(token);
                    write_byte(addr, value);
                    printf("write(0x%"PRIx16", %"PRId8")\r\n", addr, value);
                }
                else{
                    printf("No value provided to write!\r\n");
                }
            }
            else{
                printf("No address provided to write!\r\n");
            }
        }
        else{
            printf("[ERR]: unknown command\r\n");
        }
    }
}

