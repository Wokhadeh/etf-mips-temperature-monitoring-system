/*
 * driver_uart.c
 *
 *  Created on: 29 Jan 2022
 *      Author: wokad
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <string.h>

#include "usart.h"

// TRANSMIT

static TaskHandle_t UART_TransmitTaskHandle;
static QueueHandle_t UART_TransmitQueueHandle;
static SemaphoreHandle_t UART_TransmitSemaphoreHandle;

static void UART_TransmitTask(void *parameters) {
	uint8_t buffer;
	while (1) {
		xQueueReceive(UART_TransmitQueueHandle, &buffer, portMAX_DELAY); // take element from buffer
		HAL_UART_Transmit_IT(&huart1, &buffer, sizeof(uint8_t)); // send element
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // wait until sending is done
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == huart1->Instance) {
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_TransmitTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}

}

// RECIEVE

static TaskHandle_t UART_RecieveTaskHandle;
static QueueHandle_t UART_RecieveQueueHandle;
static SemaphoreHandle_t UART_RecieveSemaphoreHandle;

static void UART_RecieveTask(void *parameters) {

	uint8_t buffer;
	while (1) {

		HAL_UART_Receive_IT(&huart1, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		xQueueSendToBack(&UART_RecieveQueueHandle, &buffer, sizeof(uint8_t));

	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == huart1->Instance) {
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_RecieveTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}

}

//TRANSMIT UTILS

void UART_TransmitCharacter(char character) {
	xSemaphoreTake(UART_TransmitSemaphoreHandle, portMAX_DELAY);

	xQueueSendToBack(&UART_TransmitQueueHandle, &character, portMAX_DELAY);

	xSemaphoreGive(UART_TransmitSemaphoreHandle);
}

void UART_TransmitString(char const *string) {
	xSemaphoreTake(UART_TransmitSemaphoreHandle, portMAX_DELAY);

	for (uint8_t i = 0; i < strlen(string); i++) {
		xQueueSendToBack(&UART_TransmitQueueHandle, string + i, portMAX_DELAY);
	}

	xSemaphoreGive(UART_TransmitSemaphoreHandle);
}

void UART_TransmitDecimal(uint32_t decimal) {
	xSemaphoreTake(UART_TransmitSemaphoreHandle, portMAX_DELAY);
	char digits[32];
	uint8_t index = 32;
	while (index >= 0 && decimal != 0) {
		digits[--index] = '0' + decimal % 10;
		decimal = decimal / 10;

	}
	for (uint8_t i = index; i < 32; i++) {
		xQueueSendToBack(&UART_TransmitQueueHandle, digits + i, portMAX_DELAY);
	}

	xSemaphoreGive(UART_TransmitSemaphoreHandle);
}

//RECIEVE UTILS

char UART_RecieveCharacter() {
	xSemaphoreTake(UART_RecieveSemaphoreHandle, portMAX_DELAY);

	char character;
	xQueueReceive(UART_RecieveQueueHandle, &character, portMAX_DELAY);

	xSemaphoreGive(UART_RecieveSemaphoreHandle);
	return character;
}

char* UART_RecieveString() {

	xSemaphoreTake(UART_RecieveSemaphoreHandle, portMAX_DELAY);

	char *string = pvPortMalloc(64 * sizeof(char));

	if (string != NULL) {
		uint8_t index = 0;
		char character = ' ';
		while (character != '\r' && index < 64) {
			xQueueReceive(UART_RecieveQueueHandle, &character, portMAX_DELAY);
			string[index++] = character;

		}
		string[--index] = '\0';

	}

	xSemaphoreGive(UART_RecieveSemaphoreHandle);
	return string;

}

uint32_t UART_RecieveDecimal() {
	xSemaphoreTake(UART_RecieveSemaphoreHandle, portMAX_DELAY);
	uint32_t decimal = 0;

	char buffer = "\0";
	while (buffer != '\r') {
		xQueueReceive(UART_RecieveQueueHandle, &buffer, portMAX_DELAY);
		if (buffer >= '0' && buffer <= '9') {
			decimal = decimal * 10 + (buffer - '0');
		}
	}

	xSemaphoreGive(UART_RecieveSemaphoreHandle);
	return decimal;
}
//Initalization

void UART_Init(){
	xTaskCreate(UART_TransmitTask, "", 64, NULL, 4, &UART_TransmitTaskHandle);
	UART_TransmitQueueHandle = xQueueCreate(64,sizeof(uint8_t));
	UART_TransmitSemaphoreHandle = xSemaphoreCreateMutex();

	xTaskCreate(UART_RecieveTask, "", 64, NULL, 20, &UART_RecieveTaskHandle);
	UART_RecieveQueueHandle = xQueueCreate(64,sizeof(uint8_t));
	UART_RecieveSemaphoreHandle = xSemaphoreCreateMutex();

}
