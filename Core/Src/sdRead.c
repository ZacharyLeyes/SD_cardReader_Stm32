#include "stm32f4xx_hal.h"
#include "main.h"
#include "sdRead.h"
#include <string.h>
#include <stdio.h>

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;

void CS_Select();
void CS_UnSelect();
void sendCommand(uint8_t *cmd, int size);

void setSpiMode() {

	uint8_t pData = 0xFF;

	CS_UnSelect();

	// Send 80 clocks with MOSI high — puts SD into SPI mode
	for (int i = 0; i < 10; i++) {
		HAL_SPI_Transmit(&hspi1, &pData, 1, 1000);
	}

	uint8_t commandZero[6];
	commandZero[0] = 0x40;
	commandZero[1] = 0x00;
	commandZero[2] = 0x00;
	commandZero[3] = 0x00;
	commandZero[4] = 0x00;
	commandZero[5] = 0x95; // crc

	sendCommand(commandZero, 6);
}

void CS_Select() {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); // Active low
}

void CS_UnSelect() {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}

void sendCommand(uint8_t *cmd, int size) {

	uint8_t response = 0xFF;
	uint8_t dummy = 0xFF;
	char buf[8];

	CS_Select();

	// Send command packet
	HAL_SPI_Transmit(&hspi1, cmd, size, 1000);

	for (int i = 0; i < 10; i++) {
		HAL_SPI_TransmitReceive(&hspi1, &dummy, &response, 1, 100);
		sprintf(buf, "%02X ", response);
		HAL_UART_Transmit(&huart2, (uint8_t*) buf, strlen(buf), 100);

		if (response == 0x01)
			break; // got something other than idle
	}

	CS_UnSelect();
}
