/*
 * driver_temp.c
 *
 *  Created on: 29 Jan 2022
 *      Author: wokad
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "adc.h"


#define MAX_VOLTAGE 5.0
#define RESOLUTION 4096

static TaskHandle_t TEMP_TaskHandle;
static QueueHandle_t TEMP_QueueHandle;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	if(hadc->Instance == hadc1->Instance){
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(TEMP_TaskHandle, &woken);
		portYIELD_FROM_ISR(woken);

	}
}

static void TEMP_Task(void* parameters){
	while(1){
		HAL_ADC_Start_IT(&hadc1);
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		float value = HAL_ADC_GetValue(&hadc1);

		value *= MAX_VOLTAGE/RESOLUTION;
		value *= 100;

		xQueueOverwrite(TEMP_QueueHandle,&value);
		vTaskDelay(pdMS_TO_TICKS(200));

	}
}

float TEMP_GetCurrentValue(){

	float result = 0.0;
	xQueuePeek(TEMP_QueueHandle, &result, portMAX_DELAY);
	return result;
}

void TEMP_Init(){
	xTaskCreate(TEMP_Task,"TEMP_Task" , 64, NULL, 2, &TEMP_TaskHandle);
	TEMP_QueueHandle = xQueueCreate(1,sizeof(float));
}
