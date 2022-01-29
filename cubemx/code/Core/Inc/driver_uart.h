/*
 * driver_uart.h
 *
 *  Created on: 29 Jan 2022
 *      Author: wokad
 */

#ifndef CORE_INC_DRIVER_UART_H_
#define CORE_INC_DRIVER_UART_H_




extern void UART_TransmitCharacter(char character);
extern void UART_TransmitString(char const *string);
extern void UART_TransmitDecimal(uint32_t decimal);

extern char UART_ReceiveCharacter();
extern char* UART_ReceiveString();
extern uint32_t UART_ReceiveDecimal();

extern void UART_Init();

#endif /* CORE_INC_DRIVER_UART_H_ */
