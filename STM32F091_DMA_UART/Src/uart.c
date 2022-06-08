/*
 * uart.c
 *
 *  Created on: Jun 6, 2022
 *      Author: Sowa
 */

#include "stm32f0xx.h"
#include "uart.h"

void uart_config() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1; //Setting P9 AND 10 on alternate function (Check pin description on datasheet), afr1=USART1
	GPIOA->AFR[1] = 1 << 4 | 1 << 8; // 0x1 for afr for port 9 and 10 is uart

	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	/* DMA1 Channel2 USART1_TX config */
	/* (1)  Peripheral address */
	/* (2)  Memory address */
	/* (3)  Memory increment */
	/*      Memory to peripheral */
	/*      8-bit transfer */
	/*      Transfer complete IT */
	DMA1_Channel2->CMAR = (uint32_t) &printf_buffer; /* (2) */
	DMA1_Channel2->CPAR = (uint32_t) &(USART1->TDR); /* (1) */
	DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_DIR; /* (3) */

	/* DMA1 Channel2 USART_RX config */
	/* (4)  Peripheral address */
	/* (5)  Memory address */
	/* (6)  Data size */
	/* (7)  Memory increment */
	/*      Peripheral to memory*/
	/*      8-bit transfer */
	/*      Transfer complete IT */
	DMA1_Channel3->CPAR = (uint32_t) &(USART1->RDR); /* (4) */
	DMA1_Channel3->CMAR = (uint32_t) &rx_buffer; /* (5) */
	DMA1_Channel3->CNDTR = 1; /* (6) */
	DMA1_Channel3->CCR |= DMA_CCR_MINC | DMA_CCR_TCIE | DMA_CCR_EN; /* (7) */

	/* Configure IT */
	/* (8) Set priority for DMA1_Channel2_3_IRQn */
	/* (9) Enable DMA1_Channel2_3_IRQn */
	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0); /* (8) */
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn); /* (9) */

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	/* Configure USART1 */
	/* (1) oversampling by 16, 9600 baud */
	/* (2) Enable DMA in reception and transmission */
	/* (3) 8 data bit, 1 start bit, 1 stop bit, no parity, reception and transmission enabled */
	USART1->BRR = 8000000 / 9600; /* (1) */
	USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; /* (2) */
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE; /* (3) */

	while ((USART1->ISR & USART_ISR_TC) != USART_ISR_TC)/* polling idle frame Transmission */
	{
		/* add time out here for a robust application */
	}
	USART1->ICR |= USART_ICR_TCCF;/* Clear TC flag */
	USART1->CR1 |= USART_CR1_TCIE;/* Enable TC interrupt */

}

void USART1_IRQHandler(void) {
//	uart1_rxbuffer[uart1_rx_buffer_position] = USART1->RDR;
//	uart1_rx_buffer_position++;
}

void send_char(char c) {
	while (!(USART1->ISR & USART_ISR_TXE))
		;
	USART1->TDR = c;
}

void send_string(const char *s) {
	while (*s)
		send_char(*s++);
}

int __io_putchar(int ch) {
	while (!(USART1->ISR & USART_ISR_TXE))
		;
	printf_buffer = ch;
	DMA1_Channel2->CCR &= ~ DMA_CCR_EN;
	DMA1_Channel2->CNDTR = 1;/* Data size */
	DMA1_Channel2->CCR |= DMA_CCR_EN;
	//USART1->TDR = ch; //For pooling transffer
	return 0;
}
