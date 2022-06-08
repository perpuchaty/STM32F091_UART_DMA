/*
 * uart.h
 *
 *  Created on: 3 gru 2020
 *      Author: bga-service
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f0xx.h"

extern uint8_t printf_buffer;
extern uint8_t rx_buffer;
extern uint8_t rx_message[256];
extern uint8_t rx_position;

void uart_config();
void send_char(char c);
void send_string(const char* s);
int __io_putchar(int ch);
#endif /* UART_H_ */
