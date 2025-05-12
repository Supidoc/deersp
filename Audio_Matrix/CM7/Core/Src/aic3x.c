/*
 * aic3x.c
 *
 *  Created on: Apr 21, 2025
 *      Author: dgrob
 */

#include "aic3x.h"

#define AIC3X_RESET_TIME_LOW 				200u
#define AIC3X_RESET_TIME_HIGH 				0u

#define AIC3X_DATAPATH_LEFT_MASK 			(0b11u << 3) 	    		/** Left DAC data path mask */
#define AIC3X_DATAPATH_RIGHT_MASK 			(0b11u << 1) 	    		/** Right DAC data path mask */

#define AIC3X_ASD_BCLK_DIRECTION_MASK 		(1u << 23)					/** BCLK direction mask */
#define AIC3X_ASD_WCLK_DIRECTION_MASK 		(1u << 22)					/** WCLK direction mask */
#define AIC3X_ASD_DOUT_TRISTATE_MASK 		(1u << 21)					/** DOUT tristate mask */
#define AIC3X_ASD_CLK_DRIVE_CTRL_MASK 		(1u << 20) 					/** CLK Drive control mask */
#define AIC3X_ASD_EFFECT_3D_MASK 			(1u << 18)					/** 3D effect mask */
#define AIC3X_ASD_TRANSFER_MODE_MASK 		(0b11u << 14)				/** Transfer mode mask */
#define AIC3X_ASD_WORD_LENGTH_MASK 			(0b11u << 12)				/** Word length mask */
#define AIC3X_ASD_BCLK_265_CLOCK_MODE_MASK 	(1u << 11)					/** BCLK 265 clock mode mask */
#define AIC3X_ASD_DAC_RESYNC_MASK 			(1u << 10)					/** DAC resync mask */
#define AIC3X_ASD_ADC_RESYNC_MASK 			(1u << 9)					/** ADC resync mask */
#define AIC3X_ASD_RESYNC_MUTE_MASK 			(1u << 8)					/** Resync mute mask */
#define AIC3X_ASD_WORD_OFFSET_MASK 			(0b11111111u)				/** Word offset mask */

#define AIC3X_PLL_Q_MASK 					(0b1111u << 3) 				/** PLL Q register bit mask */
#define AIC3X_PLL_P_MASK 					(0b111u) 			        /** PLL P register bit mask */
#define AIC3X_PLL_J_MASK 					(0b111111u << 2)			/** PLL J register bit mask */
#define AIC3X_PLL_D_MASK 					(0b11111111111111u << 2) 	/** PLL D register bit mask (for both of the registers combined) */
#define AIC3X_PLL_R_MASK 					(0b1111u)			        /** PLL R register bit mask */

#define AIC3X_PGA_GAIN_MUTED_MASK 			(1u << 7)			        /** PGA gain enable mask */
#define AIC3X_PGA_GAIN_MASK 				(0b1111111u)			    /** PGA gain mask */

#define AIC3X_LEFT_ADC_MIC2L_GAIN_MASK (0b1111u << 4)	/** Left ADC MIC2L gain mask */
#define AIC3X_LEFT_ADC_MIC2R_GAIN_MASK (0b1111u)		/** Left ADC MIC2R gain mask */
#define AIC3X_RIGHT_ADC_MIC2L_GAIN_MASK (0b1111u << 4)	/** Right ADC MIC2L gain mask */
#define AIC3X_RIGHT_ADC_MIC2R_GAIN_MASK (0b1111u)		/** Right ADC MIC2R gain mask */
#define AIC3X_LEFT_ADC_MIC1LP_DIFFERENTIAL_MASK (1u << 7)	/** Left ADC MIC1LP differential mask */
#define AIC3X_LEFT_ADC_MIC1LP_GAIN_MASK (0b1111u << 3)	/** Left ADC MIC1LP gain mask */
#define AIC3X_LEFT_ADC_MIC1LP_POWER_MASK (1u << 2)	/** Left ADC MIC1LP power mask */
#define AIC3X_LEFT_ADC_MIC1LP_SOFT_STEPPING_MASK (0b11u)	/** Left ADC MIC1LP soft stepping mask */
#define AIC3X_LEFT_ADC_MIC1RP_DIFFERENTIAL_MASK (1u << 7)	/** Left ADC MIC1RP differential mask */
#define AIC3X_LEFT_ADC_MIC1RP_GAIN_MASK (0b1111u << 3)	/** Left ADC MIC1RP gain mask */
#define AIC3X_RIGHT_ADC_MIC1RP_DIFFERENTIAL_MASK (1u << 7)	/** Right ADC MIC1RP differential mask */
#define AIC3X_RIGHT_ADC_MIC1RP_GAIN_MASK (0b1111u << 3)	/** Right ADC MIC1RP gain mask */
#define AIC3X_RIGHT_ADC_MIC1RP_POWER_MASK (1u << 2)	/** Right ADC MIC1RP power mask */
#define AIC3X_RIGHT_ADC_MIC1RP_SOFT_STEPPING_MASK (0b11u)	/** Right ADC MIC1RP soft stepping mask */
#define AIC3X_RIGHT_ADC_MIC1LP_DIFFERENTIAL_MASK (1u << 7)	/** Right ADC MIC1LP differential mask */
#define AIC3X_RIGHT_ADC_MIC1LP_GAIN_MASK (0b1111u << 3)	/** Right ADC MIC1LP gain mask */

#define AIC3X_LEFT_DAC_POWER_MASK (1u << 7)	/** Left DAC power mask */
#define AIC3X_RIGHT_DAC_POWER_MASK (1u << 6)	/** Right DAC power mask */

#define AIC3X_VOL_MUTED_MASK (1u << 7)	/** Volume control muted mask */
#define AIC3X_VOL_MASK (0b1111111u)	/** Volume control volume mask */

#define AIC3X_OUTPUT_LEVEL_MASK (0b1111u << 4)	/** Output level mask */
#define AIC3X_OUTPUT_MUTED_MASK (1u << 3)	/** Output muted mask */
#define AIC3X_OUTPUT_DRIVE_CTRL_MASK (1u << 2)	/** Output power-down drive control mask */
#define AIC3X_OUTPUT_VOLUME_CTRL_STATUS_MASK (1u << 1)	/** Output volume control status mask */
#define AIC3X_OUTPUT_POWER_CONTROL_MASK (1u)	/** Output power control mask */

/**
 * @brief Initialize the AIC3X
 *
 * This function performs a hardware reset and a software reset of the AIC3X.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X
 * @retval Hal Status
 */
HAL_StatusTypeDef AIC3X_Init(AIC3X_HandleTypeDef *haic3x) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	AIC3X_HwReset(haic3x);
	return AIC3X_SwReset(haic3x);
}
/**
 * @brief Performs hardware reset
 *
 * Performs a hardware reset of the AIC3X by toggling the reset pin.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @retval Hal Status
 */
HAL_StatusTypeDef AIC3X_HwReset(AIC3X_HandleTypeDef *haic3x) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	HAL_GPIO_WritePin(haic3x->rst_port, haic3x->rst_pin, GPIO_PIN_RESET);
	HAL_Delay(AIC3X_RESET_TIME_LOW);

	HAL_GPIO_WritePin(haic3x->rst_port, haic3x->rst_pin, GPIO_PIN_SET);
	HAL_Delay(AIC3X_RESET_TIME_HIGH);
	return HAL_OK;
}

/**
 * @brief Performs software reset
 *
 * Performs a software reset of the AIC3X by writing to the reset register.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @return Hal Status
 */
HAL_StatusTypeDef AIC3X_SwReset(AIC3X_HandleTypeDef *haic3x) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	uint8_t writeBuffer = 0x80u;
	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_RESET, I2C_MEMADD_SIZE_8BIT, &writeBuffer, 1U, 10);
}

HAL_StatusTypeDef AIC3X_ReadData(AIC3X_HandleTypeDef *haic3x, uint8_t reg,
		uint8_t *data, uint8_t dataSize) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (data == NULL) {
		return HAL_ERROR;
	}

	return HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1, reg,
	I2C_MEMADD_SIZE_8BIT, data, dataSize, 10);
}

HAL_StatusTypeDef AIC3X_WriteData(AIC3X_HandleTypeDef *haic3x, uint8_t reg,
		uint8_t *data, uint8_t dataSize) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (data == NULL) {
		return HAL_ERROR;
	}

	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1, reg,
	I2C_MEMADD_SIZE_8BIT, data, dataSize, 10);
}

HAL_StatusTypeDef AIC3X_ReadDataPathControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DataPathControl_TypeDef *pDataPathControl) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pDataPathControl == NULL) {
		return HAL_ERROR;
	}

	uint8_t readBuffer;
	HAL_StatusTypeDef status;

	status = AIC3X_ReadData(haic3x, AIC3X_CODEC_DATAPATH_REG, &readBuffer,
			sizeof(readBuffer));

	if (status != HAL_OK) {
		return status;
	}

	pDataPathControl->LeftDACDataPath = (AIC3X_DataPath_TypeDef) ((readBuffer
			& AIC3X_DATAPATH_LEFT_MASK) >> 3);
	pDataPathControl->RightDACDataPath = (AIC3X_DataPath_TypeDef) ((readBuffer
			& AIC3X_DATAPATH_RIGHT_MASK) >> 1);

	return HAL_OK;
}

HAL_StatusTypeDef AIC3X_WriteDataPathControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DataPathControl_TypeDef *pDataPathControl) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pDataPathControl == NULL) {
		return HAL_ERROR;
	}

	uint8_t readBuffer;
	uint8_t writeBuffer;

	readBuffer = AIC3X_ReadData(haic3x, AIC3X_CODEC_DATAPATH_REG, &readBuffer,
			sizeof(readBuffer));

	writeBuffer = readBuffer
			& (~(AIC3X_DATAPATH_LEFT_MASK | AIC3X_DATAPATH_RIGHT_MASK));

	writeBuffer |= (uint8_t) pDataPathControl->LeftDACDataPath << 3;
	writeBuffer |= (uint8_t) pDataPathControl->RightDACDataPath << 1;

	return AIC3X_WriteData(haic3x, AIC3X_CODEC_DATAPATH_REG, &writeBuffer,
			sizeof(writeBuffer));
}

/**
 * @brief Reads the ASD interface control and stores it in a given struct.
 *
 * Reads the ASD interface control from the ASD interface control register
 * converts the bit values to the corresponding struct member values and stores
 * them in a given struct.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pASDIntfCtrl Pointer to a AIC3X_ASDIntCtrl_TypeDef structure that
 * 	will be populated with the ASD interface control data.
 * @return Hal Status
 *
 * @warning The size of the sent data is not calculated with sizeof() but set
 * 	with a constant value of 3u because the registers have a length of 24 bits.
 */
HAL_StatusTypeDef AIC3X_ReadASDataIntfControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ASDIntCtrl_TypeDef *pASDIntfCtrl) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pASDIntfCtrl == NULL) {
		return HAL_ERROR;
	}

	uint32_t readBuffer;
	HAL_StatusTypeDef status;

	/* read ASD interface control register) */
	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_ASD_INTF_CTRLA, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &readBuffer, 3u, 10);

	if (status != HAL_OK) {
		return status;
	}

	/* fill the ASDIntfCtrl struct with the read values */
	pASDIntfCtrl->BclkOutput = (readBuffer & AIC3X_ASD_BCLK_DIRECTION_MASK)
			>> 24;
	pASDIntfCtrl->WclkOutput = (readBuffer & AIC3X_ASD_WCLK_DIRECTION_MASK)
			>> 23;
	pASDIntfCtrl->DoutTristate = (readBuffer & AIC3X_ASD_DOUT_TRISTATE_MASK)
			>> 22;
	pASDIntfCtrl->ClkDriveCtrl = (readBuffer & AIC3X_ASD_CLK_DRIVE_CTRL_MASK)
			>> 21;
	pASDIntfCtrl->Effect3D = (readBuffer & AIC3X_ASD_EFFECT_3D_MASK) >> 19;
	pASDIntfCtrl->TransferMode = (AIC3X_ASD_TransferMode_TypeDef) ((readBuffer
			& AIC3X_ASD_TRANSFER_MODE_MASK) >> 14);
	pASDIntfCtrl->WordLength = (AIC3X_ASD_WordLength_TypeDef) ((readBuffer
			& AIC3X_ASD_WORD_LENGTH_MASK) >> 12);
	pASDIntfCtrl->Bclk265ClockMode = (readBuffer
			& AIC3X_ASD_BCLK_265_CLOCK_MODE_MASK) >> 11;
	pASDIntfCtrl->DACResync = (readBuffer & AIC3X_ASD_DAC_RESYNC_MASK) >> 10;
	pASDIntfCtrl->ADCResync = (readBuffer & AIC3X_ASD_ADC_RESYNC_MASK) >> 9;
	pASDIntfCtrl->ReSyncMute = (readBuffer & AIC3X_ASD_RESYNC_MUTE_MASK) >> 8;
	pASDIntfCtrl->WordOffset = (readBuffer & AIC3X_ASD_WORD_OFFSET_MASK);

	return HAL_OK;
}

/**
 * @brief Writes the ASD interface control config to the AIC3X
 *
 * Sets the ASD interface control by first defining the needed register values
 * and then writing them to the registers.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pASDIntfCtrl Pointer to a AIC3X_ASDIntCtrl_TypeDef structure
 * 	which defines the ASD interface control.
 * @return Hal Status
 *
 * @warning The size of the sent data is not calculated with sizeof() but set
 * 	with a constant value of 3u because the registers have a length of 24 bits.
 */
HAL_StatusTypeDef AIC3X_WriteASDataIntfControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ASDIntCtrl_TypeDef *pASDIntfCtrl) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pASDIntfCtrl == NULL) {
		return HAL_ERROR;
	}

	uint32_t writeBuffer = 0;

	/*Set corresponding bits for the values in pASDIntfCtrl */
	writeBuffer |= pASDIntfCtrl->BclkOutput << 24;
	writeBuffer |= pASDIntfCtrl->WclkOutput << 23;
	writeBuffer |= pASDIntfCtrl->DoutTristate << 22;
	writeBuffer |= pASDIntfCtrl->ClkDriveCtrl << 21;
	writeBuffer |= pASDIntfCtrl->Effect3D << 19;
	writeBuffer |= pASDIntfCtrl->TransferMode << 14;
	writeBuffer |= pASDIntfCtrl->WordLength << 12;
	writeBuffer |= pASDIntfCtrl->Bclk265ClockMode << 11;
	writeBuffer |= pASDIntfCtrl->DACResync << 10;
	writeBuffer |= pASDIntfCtrl->ADCResync << 9;
	writeBuffer |= pASDIntfCtrl->ReSyncMute << 8;
	writeBuffer |= pASDIntfCtrl->WordOffset;

	/* Shift the writeBuffer to the left by 8 bits to make place for the bits of the ASD overflow control register*/
	writeBuffer = writeBuffer << 8;

	/* Write the ASD interface control register */
	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_ASD_INTF_CTRLA, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &writeBuffer, 3u, 10);

}
/**
 * @brief Reads the PLL programming and stores it in a given struct.
 *
 * Reads the PLL programming from the pll programming and the codec overflow register
 * and stores it in a given struct.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pPllProgramming Pointer to a AIC3X_PLLProgramming_Typedef structure that
 * 	will be populated with the PLL programming
 * @return Hal Status
 *
 * @warning This function will reset the codec overflow register since it needs to
 * 	read the PLL R value which is the same register.
 */
HAL_StatusTypeDef USER_AIC3X_ReadPllProgramming(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PLLProgramming_TypeDef *pPllProgramming) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pPllProgramming == NULL) {
		return HAL_ERROR;
	}

	uint32_t pllRegisterReadBuffer;
	HAL_StatusTypeDef status;

	/* read data-path register */
	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_PLL_PROGA_REG, I2C_MEMADD_SIZE_8BIT,
			(uint8_t*) &pllRegisterReadBuffer, sizeof(pllRegisterReadBuffer),
			10);

	if (status != HAL_OK) {
		return status;
	}

	uint8_t overflowRegisterReadBuffer;

	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_OVRF_STATUS_AND_PLLR_REG, I2C_MEMADD_SIZE_8BIT,
			(uint8_t*) &overflowRegisterReadBuffer,
			sizeof(overflowRegisterReadBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	uint8_t pllQRegisterValue = pllRegisterReadBuffer >> 24 & AIC3X_PLL_Q_MASK;
	uint8_t pllPRegisterValue = pllRegisterReadBuffer >> 24 & AIC3X_PLL_P_MASK;
	uint8_t pllJRegisterValue = pllRegisterReadBuffer >> 16 & AIC3X_PLL_J_MASK;
	uint16_t pllDRegisterValue = (uint16_t) pllRegisterReadBuffer
			& AIC3X_PLL_D_MASK;
	uint8_t pllRRegisterValue = overflowRegisterReadBuffer & AIC3X_PLL_R_MASK;

	bool enable = (bool) (pllRegisterReadBuffer >> 31);

	/* Set pllQ to integer value of register bits except for 0b0000 and 1 which are 16 and 17 */
	uint8_t pllQ =
			(pllQRegisterValue >> 3) < 2 ?
					(pllQRegisterValue >> 3) + 16 : (pllQRegisterValue >> 3);

	/* Set pllP to integer value of register bits except for 0b000 which is 8 */
	uint8_t pllP = pllPRegisterValue < 1 ? 8u : pllPRegisterValue;

	/* Set pllP to integer value of register bits */
	uint8_t pllJ = pllJRegisterValue >> 2;

	/* Set pllD to integer value of register bits */
	uint16_t pllD = pllDRegisterValue >> 2;

	/* Set pllR to integer value of register bits except for 0b0000 which is 16*/
	uint8_t pllR = pllRRegisterValue < 1 ? 16u : pllRRegisterValue;

	if (pllQ > 17 || pllP > 8 || pllJ > 63 || pllD > 9999 || pllR > 16) {
		return HAL_ERROR;
	}
	pPllProgramming->Enable = enable;
	pPllProgramming->PllQ = pllQ;
	pPllProgramming->PllP = pllP;
	pPllProgramming->PllJ = pllJ;
	pPllProgramming->PllD = pllD;
	pPllProgramming->PllR = pllR;

	return HAL_OK;
}

/**
 * @brief Writes the pll programming config to the registers
 *
 * Writes the PLL programming to the registers by defining the
 * register values for the pll programming register and reading the codec overflow
 * register and then combining it with with the pll R values and then writing it
 * back to the registers.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *                the configuration information for the specified AIC3X.
 * @param pPllProgramming Pointer to a AIC3X_PLLProgramming_Typedef structure
 * 		  				   which defines the PLL programming. The values will be checked.
 * @return Hal Status
 */

HAL_StatusTypeDef USER_AIC3X_WritePllProgramming(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PLLProgramming_TypeDef *pPllProgramming) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pPllProgramming == NULL) {
		return HAL_ERROR;
	}

	/* Check if the values are within the valid range */
	if (pPllProgramming->PllQ > 17 || pPllProgramming->PllQ < 2
			|| pPllProgramming->PllP > 8 || pPllProgramming->PllP < 1
			|| pPllProgramming->PllJ > 63 || pPllProgramming->PllJ < 1
			|| pPllProgramming->PllD > 9999 || pPllProgramming->PllR > 16) {
		return HAL_ERROR;
	}

	uint32_t pllRegisterWriteBuffer = 0;
	uint32_t overflowRegisterWriteBuffer = 0;

	/* Set the enable bit */
	pllRegisterWriteBuffer |= pPllProgramming->Enable << 31;

	/* Set pllQ register bits from pllQ integer value except for 16 and 17 which are 0b0000 and 0b0001 */
	pllRegisterWriteBuffer |= (
			pPllProgramming->PllQ > 15 ?
					pPllProgramming->PllQ : pPllProgramming->PllQ - 16) << 24;

	/* Set pllP register bits from pllP integer value except for 8 which is 0b000 */
	pllRegisterWriteBuffer |= (
			pPllProgramming->PllP > 7 ? 0u : pPllProgramming->PllP) << 21;

	/* Set pllJ register bits from pllJ integer value */
	pllRegisterWriteBuffer |= pPllProgramming->PllJ << 18;

	/* Set pllD register bits from pllD integer value */
	pllRegisterWriteBuffer |= pPllProgramming->PllD << 2;

	/* Set pllR register bits from pllR integer value except for 16 which is 0b0000 */

	overflowRegisterWriteBuffer = ((
			pPllProgramming->PllR > 15 ? 0u : pPllProgramming->PllR)
			& AIC3X_PLL_R_MASK);

	HAL_StatusTypeDef status;

	/* Set the PLL programming register */
	status = HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_PLL_PROGA_REG, I2C_MEMADD_SIZE_8BIT,
			(uint8_t*) &pllRegisterWriteBuffer, sizeof(pllRegisterWriteBuffer),
			10);

	if (status != HAL_OK) {
		return status;
	}

	/* Set the overflow register */
	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_OVRF_STATUS_AND_PLLR_REG, I2C_MEMADD_SIZE_8BIT,
			(uint8_t*) &overflowRegisterWriteBuffer,
			sizeof(overflowRegisterWriteBuffer), 10);

}

HAL_StatusTypeDef AIC3X_ReadPGAGainControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PGAGainControl_TypeDef *pPGAGainControl) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pPGAGainControl == NULL) {
		return HAL_ERROR;
	}

	uint16_t readBuffer;
	HAL_StatusTypeDef status;

	status = AIC3X_ReadData(haic3x, LADC_VOL, (uint8_t*) &readBuffer,
			sizeof(readBuffer));

	if (status != HAL_OK) {
		return status;
	}

	pPGAGainControl->LADCMuted = (readBuffer >> 8 & AIC3X_PGA_GAIN_MUTED_MASK)
			>> 7;
	pPGAGainControl->LADCGain = (readBuffer >> 8) & AIC3X_PGA_GAIN_MASK;
	pPGAGainControl->RADCMuted = (readBuffer & AIC3X_PGA_GAIN_MUTED_MASK) >> 7;
	pPGAGainControl->RADCGain = readBuffer & AIC3X_PGA_GAIN_MASK;

	return HAL_OK;
}

HAL_StatusTypeDef AIC3X_WritePGAGainControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PGAGainControl_TypeDef *pPGAGainControl) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pPGAGainControl == NULL) {
		return HAL_ERROR;
	}

	uint16_t writeBuffer = 0;

	writeBuffer |= pPGAGainControl->LADCMuted << 15;
	writeBuffer |= (pPGAGainControl->LADCGain & AIC3X_PGA_GAIN_MASK) << 8;
	writeBuffer |= pPGAGainControl->RADCMuted << 7;
	writeBuffer |= pPGAGainControl->RADCGain & AIC3X_PGA_GAIN_MASK;

	return AIC3X_WriteData(haic3x, LADC_VOL, (uint8_t*) &writeBuffer,
			sizeof(writeBuffer));
}

HAL_StatusTypeDef AIC3X_ReadADCMixControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ADCMixControl_TypeDef *pADCMixControl) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pADCMixControl == NULL) {
		return HAL_ERROR;
	}

	uint16_t readBufferMIC2;
	uint8_t readBufferLeftADCMIC1LP;
	uint16_t readBufferMIC1RP;
	uint8_t readBufferRightADCMIC1LP;

	HAL_StatusTypeDef status;

	status = AIC3X_ReadData(haic3x, MIC2LR_LADC_CTRL,
			(uint8_t*) &readBufferMIC2, sizeof(readBufferMIC2));

	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_ReadData(haic3x, MIC1L_LADC_CTRL, &readBufferLeftADCMIC1LP,
			sizeof(readBufferLeftADCMIC1LP));
	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_ReadData(haic3x, MIC1R_LADC_CTRL,
			(uint8_t*) &readBufferMIC1RP, sizeof(readBufferMIC1RP));
	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_ReadData(haic3x, MIC1L_RADC_CTRL, &readBufferRightADCMIC1LP,
			sizeof(readBufferRightADCMIC1LP));

	if (status != HAL_OK) {
		return status;
	}

	pADCMixControl->LeftADC_MIC2LGain = (readBufferMIC2
			& AIC3X_LEFT_ADC_MIC2L_GAIN_MASK) >> 12;
	pADCMixControl->LeftADC_MIC2RGain = readBufferMIC2
			& AIC3X_LEFT_ADC_MIC2R_GAIN_MASK >> 8;
	pADCMixControl->RightADC_MIC2LGain = (readBufferMIC2
			& AIC3X_RIGHT_ADC_MIC2L_GAIN_MASK) >> 4;
	pADCMixControl->RightADC_MIC2RGain = readBufferMIC2
			& AIC3X_RIGHT_ADC_MIC2R_GAIN_MASK;

	pADCMixControl->LeftADC_MIC1LPDifferential = (readBufferLeftADCMIC1LP
			& AIC3X_LEFT_ADC_MIC1LP_DIFFERENTIAL_MASK) >> 7;
	pADCMixControl->LeftADC_MIC1LPGain = (readBufferLeftADCMIC1LP
			& AIC3X_LEFT_ADC_MIC1LP_GAIN_MASK) >> 3;
	pADCMixControl->LeftADC_PowerUp = (readBufferLeftADCMIC1LP
			& AIC3X_LEFT_ADC_MIC1LP_POWER_MASK) >> 2;
	pADCMixControl->LeftADC_PGASoftStepping = readBufferLeftADCMIC1LP
			& AIC3X_LEFT_ADC_MIC1LP_SOFT_STEPPING_MASK;

	pADCMixControl->LeftADC_MIC1RPDifferential = (readBufferMIC1RP
			& AIC3X_LEFT_ADC_MIC1RP_DIFFERENTIAL_MASK) >> 15;
	pADCMixControl->LeftADC_MIC1RPGain = (readBufferMIC1RP
			& AIC3X_LEFT_ADC_MIC1RP_GAIN_MASK) >> 11;
	pADCMixControl->RightADC_MIC1RPDifferential = (readBufferMIC1RP
			& AIC3X_RIGHT_ADC_MIC1RP_DIFFERENTIAL_MASK) >> 7;
	pADCMixControl->RightADC_MIC1RPGain = (readBufferMIC1RP
			& AIC3X_RIGHT_ADC_MIC1RP_GAIN_MASK) >> 3;
	pADCMixControl->RightADC_PowerUp = (readBufferMIC1RP
			& AIC3X_RIGHT_ADC_MIC1RP_POWER_MASK) >> 2;
	pADCMixControl->RightADC_PGASoftStepping = readBufferMIC1RP
			& AIC3X_RIGHT_ADC_MIC1RP_SOFT_STEPPING_MASK;

	pADCMixControl->RightADC_MIC1LPDifferential = (readBufferRightADCMIC1LP
			& AIC3X_RIGHT_ADC_MIC1LP_DIFFERENTIAL_MASK) >> 7;
	pADCMixControl->RightADC_MIC1LPGain = (readBufferRightADCMIC1LP
			& AIC3X_RIGHT_ADC_MIC1LP_GAIN_MASK) >> 3;

	return HAL_OK;
}

HAL_StatusTypeDef AIC3X_WriteADCMixControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ADCMixControl_TypeDef *pADCMixControl) {
	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pADCMixControl == NULL) {
		return HAL_ERROR;
	}

	if ((pADCMixControl->LeftADC_MIC2LGain > 8
			&& pADCMixControl->LeftADC_MIC2LGain < 15)
			|| (pADCMixControl->LeftADC_MIC2RGain > 8
					&& pADCMixControl->LeftADC_MIC2RGain < 15)
			|| (pADCMixControl->RightADC_MIC2LGain > 8
					&& pADCMixControl->RightADC_MIC2LGain < 15)
			|| (pADCMixControl->RightADC_MIC2RGain > 8
					&& pADCMixControl->RightADC_MIC2RGain < 15)
			|| (pADCMixControl->LeftADC_MIC1LPGain > 8
					&& pADCMixControl->LeftADC_MIC1LPGain < 15)
			|| (pADCMixControl->LeftADC_MIC1RPGain > 8
					&& pADCMixControl->LeftADC_MIC1RPGain < 15)
			|| (pADCMixControl->RightADC_MIC1LPGain > 8
					&& pADCMixControl->RightADC_MIC1LPGain < 15)
			|| (pADCMixControl->RightADC_MIC1RPGain > 8
					&& pADCMixControl->RightADC_MIC1RPGain < 15)) {
		return HAL_ERROR;
	}

uint16_t writeBufferMIC2 = 0;
uint8_t writeBufferLeftADCMIC1LP = 0;
uint16_t writeBufferMIC1RP = 0;
uint8_t writeBufferRightADCMIC1LP = 0;

writeBufferMIC2 |= pADCMixControl->LeftADC_MIC2LGain << 12;
writeBufferMIC2 |= pADCMixControl->LeftADC_MIC2RGain << 8;
writeBufferMIC2 |= pADCMixControl->RightADC_MIC2LGain << 4;
writeBufferMIC2 |= pADCMixControl->RightADC_MIC2RGain;

writeBufferLeftADCMIC1LP |= pADCMixControl->LeftADC_MIC1LPDifferential << 7;
writeBufferLeftADCMIC1LP |= pADCMixControl->LeftADC_MIC1LPGain << 3;
writeBufferLeftADCMIC1LP |= pADCMixControl->LeftADC_PowerUp << 2;
writeBufferLeftADCMIC1LP |= pADCMixControl->LeftADC_PGASoftStepping & AIC3X_LEFT_ADC_MIC1LP_SOFT_STEPPING_MASK;

writeBufferMIC1RP |= pADCMixControl->LeftADC_MIC1RPDifferential << 15;
writeBufferMIC1RP |= pADCMixControl->LeftADC_MIC1RPGain << 11;
writeBufferMIC1RP |= pADCMixControl->RightADC_MIC1RPDifferential << 7;
writeBufferMIC1RP |= pADCMixControl->RightADC_MIC1RPGain << 3;
writeBufferMIC1RP |= pADCMixControl->RightADC_PowerUp << 2;
writeBufferMIC1RP |= pADCMixControl->RightADC_PGASoftStepping & AIC3X_RIGHT_ADC_MIC1RP_SOFT_STEPPING_MASK;

writeBufferRightADCMIC1LP |= pADCMixControl->RightADC_MIC1LPDifferential
<< 7;
writeBufferRightADCMIC1LP |= pADCMixControl->RightADC_MIC1LPGain << 3;

HAL_StatusTypeDef status;

status = AIC3X_WriteData(haic3x, MIC2LR_LADC_CTRL, (uint8_t*) &writeBufferMIC2,
		sizeof(writeBufferMIC2));

if (status != HAL_OK) {
	return status;
}

status = AIC3X_WriteData(haic3x, MIC1L_LADC_CTRL, &writeBufferLeftADCMIC1LP,
		sizeof(writeBufferLeftADCMIC1LP));

if (status != HAL_OK) {
	return status;
}

status = AIC3X_WriteData(haic3x, MIC1R_LADC_CTRL, (uint8_t*) &writeBufferMIC1RP,
		sizeof(writeBufferMIC1RP));

if (status != HAL_OK) {
	return status;
}

status = AIC3X_WriteData(haic3x, MIC1L_RADC_CTRL, &writeBufferRightADCMIC1LP,
		sizeof(writeBufferRightADCMIC1LP));

if (status != HAL_OK) {
	return status;
}

return HAL_OK;
}

HAL_StatusTypeDef AIC3X_ReadDACPowerControl(AIC3X_HandleTypeDef *haic3x,
	AIC3X_DACPowerControl_TypeDef *pDACPowerControl) {
if (haic3x == NULL) {
	return HAL_ERROR;
}

if (pDACPowerControl == NULL) {
	return HAL_ERROR;
}

uint8_t readBuffer;
HAL_StatusTypeDef status;

status = AIC3X_ReadData(haic3x, DAC_PWR, &readBuffer, sizeof(readBuffer));

if (status != HAL_OK) {
	return status;
}

pDACPowerControl->LeftDACPoweredUp = (readBuffer & AIC3X_LEFT_DAC_POWER_MASK)
		>> 7;
pDACPowerControl->RightDACPoweredUp = (readBuffer & AIC3X_RIGHT_DAC_POWER_MASK)
		>> 6;

return HAL_OK;
}

HAL_StatusTypeDef AIC3X_WriteDACPowerControl(AIC3X_HandleTypeDef *haic3x,
	AIC3X_DACPowerControl_TypeDef *pDACPowerControl) {
if (haic3x == NULL) {
	return HAL_ERROR;
}

if (pDACPowerControl == NULL) {
	return HAL_ERROR;
}

uint8_t readBuffer;
HAL_StatusTypeDef status;

status = AIC3X_ReadData(haic3x, DAC_PWR, &readBuffer, sizeof(readBuffer));

if (status != HAL_OK) {
	return status;
}

uint8_t writeBuffer = readBuffer
		& (~(AIC3X_LEFT_DAC_POWER_MASK | AIC3X_RIGHT_DAC_POWER_MASK));

writeBuffer |= pDACPowerControl->LeftDACPoweredUp << 7;
writeBuffer |= pDACPowerControl->RightDACPoweredUp << 6;

return AIC3X_WriteData(haic3x, DAC_PWR, &writeBuffer, sizeof(writeBuffer));
}

HAL_StatusTypeDef AIC3X_ReadVolControl(AIC3X_HandleTypeDef *haic3x, uint8_t reg,
	AIC3X_VolControl_TypeDef *pVolControl) {
if (haic3x == NULL) {
	return HAL_ERROR;
}

if (pVolControl == NULL) {
	return HAL_ERROR;
}

uint16_t readBuffer;
HAL_StatusTypeDef status;

status = AIC3X_ReadData(haic3x, reg, (uint8_t*) &readBuffer,
		sizeof(readBuffer));

if (status != HAL_OK) {
	return status;
}
if(reg == LDAC_VOL || reg == RDAC_VOL)
{
	pVolControl->Muted = (readBuffer & AIC3X_VOL_MUTED_MASK) >> 7;
}
else
{
	pVolControl->Muted = (~readBuffer & AIC3X_VOL_MUTED_MASK) >> 7;
}
pVolControl->Muted = (readBuffer & AIC3X_VOL_MUTED_MASK) >> 7;
pVolControl->Volume = readBuffer & AIC3X_VOL_MASK;

return HAL_OK;
}

HAL_StatusTypeDef AIC3X_WriteVolControl(AIC3X_HandleTypeDef *haic3x,
	uint8_t reg, AIC3X_VolControl_TypeDef *pVolControl) {
if (haic3x == NULL) {
	return HAL_ERROR;
}

if (pVolControl == NULL) {
	return HAL_ERROR;
}

if (pVolControl->Volume > 127) {
	return HAL_ERROR;
}

uint8_t writeBuffer = 0;
if(reg == LDAC_VOL || reg == RDAC_VOL)
{
	writeBuffer |= pVolControl->Muted << 7;
}
else
{
	writeBuffer |= ((!pVolControl->Muted ) << 7) & AIC3X_VOL_MUTED_MASK ;
}
writeBuffer |= pVolControl->Volume & AIC3X_VOL_MASK;

return AIC3X_WriteData(haic3x, reg, (uint8_t*) &writeBuffer,
		sizeof(writeBuffer));
}

HAL_StatusTypeDef AIC3X_ReadOutputLevelControl(AIC3X_HandleTypeDef *haic3x,
	uint8_t reg, AIC3X_OutputLevelControl_TypeDef *pOutputLevelControl) {
if (haic3x == NULL) {
	return HAL_ERROR;
}

if (pOutputLevelControl == NULL) {
	return HAL_ERROR;
}

uint8_t readBuffer;
HAL_StatusTypeDef status;

status = AIC3X_ReadData(haic3x, reg, &readBuffer, sizeof(readBuffer));

if (status != HAL_OK) {
	return status;
}

pOutputLevelControl->Level = (readBuffer & AIC3X_OUTPUT_LEVEL_MASK) >> 4;
pOutputLevelControl->Muted = (readBuffer & AIC3X_OUTPUT_MUTED_MASK) >> 3;
pOutputLevelControl->PowerDownDriveControl = (readBuffer & AIC3X_OUTPUT_DRIVE_CTRL_MASK)
		>> 2;
pOutputLevelControl->VolumeControlStatus = (readBuffer
		& AIC3X_OUTPUT_VOLUME_CTRL_STATUS_MASK) >> 1;
pOutputLevelControl->PowerControl =
		readBuffer & AIC3X_OUTPUT_POWER_CONTROL_MASK;

return HAL_OK;
}

HAL_StatusTypeDef AIC3X_WriteOutputLevelControl(AIC3X_HandleTypeDef *haic3x,
	uint8_t reg, AIC3X_OutputLevelControl_TypeDef *pOutputLevelControl) {
if (haic3x == NULL) {
	return HAL_ERROR;
}

if (pOutputLevelControl == NULL) {
	return HAL_ERROR;
}

if(pOutputLevelControl->Level > 9)
{
	return HAL_ERROR;
}

uint8_t writeBuffer = 0;

writeBuffer |= pOutputLevelControl->Level << 4;
writeBuffer |= (!pOutputLevelControl->Muted) << 3;
writeBuffer |= pOutputLevelControl->PowerDownDriveControl << 2;
writeBuffer |= pOutputLevelControl->VolumeControlStatus << 1;
writeBuffer |= pOutputLevelControl->PowerControl;

return AIC3X_WriteData(haic3x, reg, (uint8_t*) &writeBuffer,
		sizeof(writeBuffer));
}
