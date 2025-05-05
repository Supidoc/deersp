/*
 * user_tca9548a.c
 *
 *  Created on: Apr 3, 2025
 *      Author: dgrob
 */

#include "user_tca9548a.h"
#include "stdint.h"

#define I2C_MUX_BASE_ADDR 112

// All times are defined using milliseconds
#define I2C_MUX_TIMEOUT 1		// Depends on bit rate. At 400kHz, 1ms should be fine
#define I2C_MUX_RESET_TIME_LOW 1	// Minimum 6ns reset pulse according to datasheet
#define I2C_MUX_RESET_TIME_HIGH 0	// Start condition can begin immediately after reset

HAL_StatusTypeDef USER_TCA9548A_Reset(TCA9548A_HandleTypeDef* mux) {
	if (mux->rst_port == NULL || mux->rst_pin == 0) return HAL_ERROR;
	
	// Pull reset pin low
	HAL_GPIO_WritePin(mux->rst_port, mux->rst_pin, GPIO_PIN_RESET);
	HAL_Delay(I2C_MUX_RESET_TIME_LOW);

	// Bring multiplexer out of reset
	HAL_GPIO_WritePin(mux->rst_port, mux->rst_pin, GPIO_PIN_SET);
	HAL_Delay(I2C_MUX_RESET_TIME_HIGH);

	// Ensure all channels are disabled by default
	return USER_TCA9548A_SelectMulti(mux, 0);
}

HAL_StatusTypeDef USER_TCA9548A_Select(TCA9548A_HandleTypeDef* mux, uint8_t ch) {

	// If ch is in range 0-7 then one channel is enabled, else all are disabled
	uint8_t mask = 1 << ch;
	return USER_TCA9548A_SelectMulti(mux, mask);
}

HAL_StatusTypeDef USER_TCA9548A_SelectMulti(TCA9548A_HandleTypeDef* mux, uint8_t mask) {
	if (mux->hi2c == NULL) return 1;

	// Transmit bitmask to multiplexer
	uint8_t addr = (I2C_MUX_BASE_ADDR + mux->addr_offset) << 1;
	HAL_StatusTypeDef res;
	res = HAL_I2C_Master_Transmit(mux->hi2c, addr, &mask, 1, I2C_MUX_TIMEOUT);
	if (res != HAL_OK) return res;

	// Read back bitmask from multiplexer to verify
	uint8_t mask_check = 0;
	res = HAL_I2C_Master_Receive(mux->hi2c, addr, &mask_check, 1, I2C_MUX_TIMEOUT);
	return res;
}
