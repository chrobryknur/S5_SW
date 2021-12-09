#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>
#include "i2c.h"
#include <string.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

// inicjalizacja UART
void uart_init(){
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // wyczyść rejestr UCSR0A
  UCSR0A = 0;
  // włącz odbiornik i nadajnik
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  // ustaw format 8n1
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream){
  // czekaj aż transmiter gotowy
  while(!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream){
  // czekaj aż znak dostępny
  while (!(UCSR0A & _BV(RXC0)));
  char c = UDR0;
  UDR0 = c;
  return c;
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

void print_data_to_I8HEX(uint16_t addr, uint8_t length, uint8_t data[]){
    uint16_t checksum = length + (addr >> 8) + (addr & 0xff);
    uint8_t  record_type = 0;
    printf(":%.2x%.4x%.2x", length, addr, record_type);
    for(uint8_t i = 0; i < length; i++){
        checksum += data[i];
        printf("%.2x", data[i]);
    }
    printf("%.2x\r\n", ((~checksum) + 1) & 0xff);
}

void read_I8HEX(uint16_t addr, uint8_t length){
    uint8_t data[length];

    i2cStart();
    i2cSend(eeprom_addr | ((addr & 0x100) >> 7)); // Select slave device and chip
    i2cSend(addr & 0xff);                         // Send address to be read from the selected chip
    i2cStart();
    i2cSend(eeprom_addr | 0x1 | ((addr & 0x100) >> 7)); // Send read request

    for(uint8_t i = 0; i < length - 1; i++){
        data[i] = i2cReadAck();                     // Read data
    }

    data[length-1] = i2cReadNoAck();                // Read data
    i2cStop();

    print_data_to_I8HEX(addr, length, data);
}

uint8_t get_hex_from_char(char c){
    if(c >= '0' && c <= '9'){
        return c - '0';
    }
    if(c >= 'a' && c <= 'f'){
        return c - 'a' + 10;
    }
    if(c >= 'A' && c <= 'F'){
        return c - 'A' + 10;
    }
    return 0;
}

uint8_t get_byte(){
    return get_hex_from_char(getchar()) << 4 | get_hex_from_char(getchar());
}

void write_I8HEX_data(uint8_t length, uint16_t addr, uint8_t data[]){
    uint8_t idx = 0;
    
    while(idx < length){
        if(idx % 4 == 0){
            _delay_ms(10);
            i2cStart();
            i2cSend(eeprom_addr | (((addr + idx) & 0x100) >> 7));
            i2cSend((addr + idx) & 0xff);
        }
        i2cSend(data[idx++]);
        if(idx % 4 == 0){
            i2cStop();
        }
    }
    if(idx-1 % 4){
        i2cStop();
    }
}

uint8_t read_I8HEX_from_uart_and_write_to_eeprom(){
    getchar(); // Delete ':'

    uint8_t length = 0;
    uint16_t addr  = 0;
    uint8_t record_type = 0;
    uint16_t calculated_checksum = 0;
    uint16_t read_checksum = 0;

    length = get_byte();
    addr = get_byte() << 8 | get_byte();
    record_type = get_byte();

    calculated_checksum = length + (addr >> 8) + (addr & 0xff);

    if(record_type){
        getchar();
        getchar();
        printf("EOT\r\n");
        return 0;
    }

    uint8_t data[length];
    for(uint8_t i = 0; i < length; i++){
        data[i] = get_byte();
        calculated_checksum += data[i];
    }
    read_checksum = get_byte();
    calculated_checksum = (~calculated_checksum) + 1;
    calculated_checksum &= 0xff;

    if(read_checksum != calculated_checksum){
        printf("[WRN]: Checksums are different 0x%.2x, 0x%.2x!\r\n", read_checksum, calculated_checksum & 0xff);
    }

    putchar('\r');
    putchar('\n');

    write_I8HEX_data(length, addr, data);
    return 1;
}

int main(){
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    i2cInit();

    uint16_t addr = 0;
    uint8_t value = 0;
    uint8_t length = 0;

    const char s[2] = " ";
    char *token;

    while(1) {
        char c;
        char buffer[32] = {'\0'};
        uint8_t idx = 0;
        while((c = getchar()) != '\r'){
            buffer[idx++] = c;
        }
        putchar('\r');
        putchar('\n');

        token = strtok(buffer, s);

        if(!strcmp(token, "read")){
            token = strtok(NULL, s);
            if(token){
                addr = atoi(token) & 0x1ff;
                token = strtok(NULL, s);
                if(token){
                    length = atoi(token);
                    read_I8HEX(addr, length);
                }
                else{
                    printf("read (0x%"PRIx16"): %"PRId8"\r\n", addr, read_byte(addr));
                    continue;
                }
            }
            else{
                printf("No address provided to read!\r\n");
                continue;
            }
        }
        else if(!strcmp(token, "write")){
            token = strtok(NULL, s);
            if(token){
                addr = atoi(token) & 0x1ff;
                token = strtok(NULL, s);

                if(token){
                    value = atoi(token);
                    write_byte(addr, value);
                    printf("write(0x%"PRIx16", %"PRId8")\r\n", addr, value);
                }
                else{
                    printf("No value provided to write!\r\n");
                    continue;
                }
            }
            else{
                while(read_I8HEX_from_uart_and_write_to_eeprom());
            }
        }
        else{
            continue;
        }
    }
}

