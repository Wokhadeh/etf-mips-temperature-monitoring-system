/*
 * driver_uart.h
 *
 *  Created on: 29 Jan 2022
 *      Author: wokad
 */

#ifndef CORE_INC_DRIVER_UART_H_
#define CORE_INC_DRIVER_UART_H_




extern void UART_AsyncTransmitCharacter(char character);
extern void UART_AsyncTransmitString(char const *string);
extern void UART_AsyncTransmitDecimal(uint32_t decimal);

extern char UART_BlockReceiveCharacter();
extern char* UART_BlockReceiveString();
extern uint32_t UART_BlockReceiveDecimal();

extern void UART_Init();

#endif /* CORE_INC_DRIVER_UART_H_ */
