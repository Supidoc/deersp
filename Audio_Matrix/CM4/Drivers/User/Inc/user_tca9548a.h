/*
 * USER_TCA9548A.h
 *
 *  Created on: Apr 3, 2025
 *      Author: dgrob
 */

#ifndef USER_TCA9548A_H_
#define USER_TCA9548A_H_

#include "stm32h7xx_hal.h"

/**
 * @defgroup TCA9548A_handle_Structure_definition TCA9548A handle Structure definition
 * @brief TCA9548A handle Structure definition
 *
 */
typedef struct __TCA9548A_HandleTypeDef {
	I2C_HandleTypeDef* hi2c;	/** I2C handle */
	GPIO_TypeDef* rst_port;		/** Reset pin GPIO port (set NULL if unused) */
	uint16_t rst_pin;		/** Reset pin bitmask */
	uint8_t addr_offset;	/**	Offset from base address (set using address pins) */
} TCA9548A_HandleTypeDef;

// Returns 0 on success, 1 on error.
HAL_StatusTypeDef USER_TCA9548A_Reset(TCA9548A_HandleTypeDef* mux);

// Assigning "ch" as 0-7 will enable the corresponding multiplexer channels.
// Any other value of "ch" will disable all channels.
// Returns 0 on success, 1 on error.
HAL_StatusTypeDef USER_TCA9548A_Select(TCA9548A_HandleTypeDef* mux, uint8_t ch);

// Multiple multiplexer channels can be enabled or disabled simultaneously.
// The 8 bits of "mask" correspond to the 8 multiplexer channels.
// If a bit is 1 then the corresponding channel is enabled, otherwise it is disabled.
// Returns 0 on success, 1 on error.
HAL_StatusTypeDef USER_TCA9548A_SelectMulti(TCA9548A_HandleTypeDef* mux, uint8_t mask);

#endif /* USER_TCA9548A_H_ */
