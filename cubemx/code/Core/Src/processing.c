/*
 * processing.c
 *
 *  Created on: 29 Jan 2022
 *      Author: wokad
 */


#include "processing.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "driver_lcd.h"
#include "driver_motor.h"
#include "driver_temp.h"
#include "driver_uart.h"

FanState fanState = TURNED_OFF;

static uint32_t tempValue;
static char tempText[4];

static void processingTask(void* parameters){
	char mess[16] = "Temperatura: ";

	LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x00);

	for(uint8_t i = 0; i < 16; i++){ //send message to LCD and virutual terminal
		LCD_CommandEnqueue(LCD_DATA, mess[i]);
		UART_TransmitCharacter(mess[i]);
	}

	while(1){
		tempValue = TEMP_GetCurrentValue();
		itoa(tempValue,tempText,10);

		FanState fanStateTarget;

		if(tempValue > 30)
			fanStateTarget = TURNED_OFF;
		else if (tempValue<35)
			fanStateTarget = SLOW;
		else
			fanStateTarget = FAST;
		for (uint32_t i = 0; i < abs(fanStateTarget - fanState); i++)
		{
			if (fanStateTarget > fanState)
			{
				MOTOR_SpeedIncrease();
			}
			else
			{
				MOTOR_SpeedDecrease();
			}
		}
				fanState = fanStateTarget;

		LCD_CommandEnqueue(LCD_INSTRUCTION,
				LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x0D);
		for (uint32_t i = 0; i < strlen(tempText); i++)
		{
			LCD_CommandEnqueue(LCD_DATA, tempText[i]);
			UART_TransmitCharacter(tempText[i]);
		}
		vTaskDelay(pdMS_TO_TICKS(200));

				// clear temperature value
		LCD_CommandEnqueue(LCD_INSTRUCTION,
				LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x0D);
		for (uint32_t i = 0; i < strlen(tempText); i++)
		{
			LCD_CommandEnqueue(LCD_DATA, ' ');
			UART_TransmitCharacter('\b');
		}

	}

}

void processingInit()
{
	LCD_Init();
	UART_Init();
	MOTOR_Init();
	TEMP_Init();
	xTaskCreate(processingTask, "processingTask", 64, NULL, 5, NULL);
}

