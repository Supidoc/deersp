/**
 * @file user_aic3x.c
 *
 * @brief Driver for the AIC3X audio codec
 *
 * @author Dominik Grob
 *
 */

#include "user_aic3x.h"

#define AIC3X_RESET_TIME_LOW 				1u
#define AIC3X_RESET_TIME_HIGH 				0u

#define AIC3X_BASE_RATE_MASK 				(1u << 7) 					/** Base rate mask */
#define AIC3X_RATE_DOUBLING_MASK 			(0b11u << 5) 				/** Rate doubling mask */

#define AIC3X_PLL_Q_MASK 					(0b1111u << 3) 				/** PLL Q register bit mask */
#define AIC3X_PLL_P_MASK 					(0b111u) 			        /** PLL P register bit mask */
#define AIC3X_PLL_J_MASK 					(0b111111u << 2)			/** PLL J register bit mask */
#define AIC3X_PLL_D_MASK 					(0b11111111111111u << 2) 	/** PLL D register bit mask (for both of the registers combined) */
#define AIC3X_PLL_R_MASK 					(0b1111u)			        /** PLL R register bit mask */

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

#define AIC3X_CODEC_DFILT_HPF_LEFT_MASK 	(0b11u << 6)				/** Left high pass filter mask */
#define AIC3X_CODEC_DFILT_HPF_RIGHT_MASK 	(0b11u << 4)				/** Right high pass filter mask */
#define AIC3X_CODEC_DFILT_DFILT_LEFT_MASK 	(1u << 3)					/** Left digital effects filter mask */
#define AIC3X_CODEC_DFILT_DEEMPH_LEFT_MASK 	(1u << 2)					/** Left de-emphasis filter mask */
#define AIC3X_CODEC_DFILT_DFILT_RIGHT_MASK 	(1u << 1)					/** Right digital effects filter mask */
#define AIC3X_CODEC_DFILT_DEEMPH_RIGHT_MASK (1u)						/** Right de-emphasis filter mask */

#define AIC3X_HEADSET_BTN_PRESS_ENABLE_MASK (1u << 15)					/** Headset button press enable mask */
#define AIC3X_HEADSET_TYPE_MASK 			(0b11u << 13)				/** Headset type mask */
#define AIC3X_DEBOUNCE_JACK_TIME_MASK 		(0b111u << 10)				/** Debounce jack time mask */
#define AIC3X_DEBOUNCE_BTN_TIME_MASK 		(0b11u << 8)				/** Debounce button time mask */
#define AIC3X_DRIVER_CAPACITIVE_COUPLING_MASK (1u << 7)					/** Driver capacitive coupling mask */
#define AIC3X_STEREO_OUTPUT_DRIVER_CONFIG_A_MASK (1u << 6)				/** Stereo output driver config A mask */
#define AIC3X_HEADSET_DETECTION_FLAG_MASK (1u << 4)						/** Headset detection flag mask */
#define AIC3X_STEREO_OUTPUT_DRIVER_CONFIG_B_MASK (1u << 3)				/** Stereo output driver config B mask */


/**
 * @brief Initialize the AIC3X
 *
 * This function performs a hardware reset and a software reset of the AIC3X.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X
 * @retval Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_Init(AIC3X_HandleTypeDef *haic3x) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	USER_AIC3X_HwReset(haic3x);
	return USER_AIC3X_SwReset(haic3x);
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
HAL_StatusTypeDef USER_AIC3X_HwReset(AIC3X_HandleTypeDef *haic3x) {

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
HAL_StatusTypeDef USER_AIC3X_SwReset(AIC3X_HandleTypeDef *haic3x) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	uint8_t writeBuffer = 0x80u;
	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_RESET, I2C_MEMADD_SIZE_8BIT, &writeBuffer, 1U, 10);
}

/**
 * @brief Reads the sample rate register
 *
 * Reads the sample rate of the ADC and DAC and stores the sample
 * rate as well as the relevant register values in a given struct.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pSampleRateRegMap Pointer to a AIC3X_SampleRateRegMap_Typedef where
 * 	the read data will be stored.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_ReadSampleRateSel(AIC3X_HandleTypeDef *haic3x,
		AIC3X_SampleRateControl_Typedef *pSampleRateControl) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pSampleRateControl == NULL) {
		return HAL_ERROR;
	}

	uint8_t rateSelReadBuffer;
	uint8_t dataPathReadBuffer;
	HAL_StatusTypeDef status;

	/* read rate selection register */
	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_SAMPLE_RATE_SEL_REG,
	I2C_MEMADD_SIZE_8BIT, &rateSelReadBuffer, sizeof(rateSelReadBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	/* Check if the ADC rate select is identical to the DAC rate select */
	if ((rateSelReadBuffer >> 4) != (rateSelReadBuffer & 0xF)) {
		return HAL_ERROR;
	}

	/* read data-path register */
	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_CODEC_DATAPATH_REG,
	I2C_MEMADD_SIZE_8BIT, &dataPathReadBuffer, sizeof(dataPathReadBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	/* Mask irrelevant bits for base rate*/
	uint8_t baseRateBuffer = dataPathReadBuffer & AIC3X_BASE_RATE_MASK;

	/* Mask irrelevant bits for rate doubling*/
	uint8_t DoubleRateBuffer = dataPathReadBuffer
			& AIC3X_RATE_DOUBLING_MASK;

	/* Check if the ADC rate doubling is identical to the DAC rate doubling */
	if (((DoubleRateBuffer >> 1) & 0b00100000)
			!= (DoubleRateBuffer & 0b00100000)) {
		return HAL_ERROR;
	}


	AIC3X_SampleRateSel_TypeDef rateSel = rateSelReadBuffer>>4;
	bool baseRate = (bool) baseRateBuffer;
	bool doubleRate = (bool) DoubleRateBuffer;

	/*Get the rate selection value by its register value */
	/*Multiply rate selection value by 10 to avoid floats */
	uint8_t sampleRateSel_times_10;

	switch (rateSel) {
	case AIC3X_S_RATE_SEL_1:
		sampleRateSel_times_10 = 10;
		break;
	case AIC3X_S_RATE_SEL_1_5:
		sampleRateSel_times_10 = 15;
		break;
	case AIC3X_S_RATE_SEL_2:
		sampleRateSel_times_10 = 20;
		break;
	case AIC3X_S_RATE_SEL_2_5:
		sampleRateSel_times_10 = 25;
		break;
	case AIC3X_S_RATE_SEL_3:
		sampleRateSel_times_10 = 30;
		break;
	case AIC3X_S_RATE_SEL_3_5:
		sampleRateSel_times_10 = 35;
		break;
	case AIC3X_S_RATE_SEL_4:
		sampleRateSel_times_10 = 40;
		break;
	case AIC3X_S_RATE_SEL_4_5:
		sampleRateSel_times_10 = 45;
		break;
	case AIC3X_S_RATE_SEL_5:
		sampleRateSel_times_10 = 50;
		break;
	case AIC3X_S_RATE_SEL_5_5:
		sampleRateSel_times_10 = 55;
		break;
	case AIC3X_S_RATE_SEL_6:
		sampleRateSel_times_10 = 60;
		break;
	default:
		return HAL_ERROR; // invalid selector
	}

	/* Calculate sampleRate */
	/* Multiply bas by 10 to cancel out the factor of ampleRateSel_times_10 */
	AIC3X_SampleRate_TypeDef sampleRate = ((baseRate ? AIC3X_FREQ_44_1_KHZ : AIC3X_FREQ_48_KHZ) * 10)
			/ sampleRateSel_times_10;
	sampleRate = doubleRate ? sampleRate * 2 : sampleRate;

	pSampleRateControl->SampleRate = sampleRate;
	pSampleRateControl->BaseRate_44_1_kHz = baseRate;
	pSampleRateControl->SampleRateSel = rateSel>>4;
	pSampleRateControl->DoubleRate = doubleRate;
	return HAL_OK;
}

/**
 * @brief Writes the sample rate config to the registers
 *
 * Sets the sample rate by first defining the needed register values and
 * then combining these with the existing values. The sample rates will be
 * identical for both ADC and DAC.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param sampleRate Sample rate to which the CODEC should be set.
 * @param pSampleRateControl Pointer to a AIC3X_SampleRateControl_Typedef structure
 * 	which will be populated with the defined values. Can be set to NULL if
 * 	not needed.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_WriteSampleRateSel(AIC3X_HandleTypeDef *haic3x,
		AIC3X_SampleRate_TypeDef sampleRate,
		AIC3X_SampleRateControl_Typedef *pSampleRateControl) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	AIC3X_SampleRateControl_Typedef sampleRateControlBuffer;
	sampleRateControlBuffer.SampleRate = sampleRate;

	/* Define the register values based on the chosen sampleRate */
	switch (sampleRate) {
	case AIC3X_FREQ_8_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_6;
		break;
	case AIC3X_FREQ_11_025_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = true;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_4;
		break;
	case AIC3X_FREQ_12_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_4;
		break;
	case AIC3X_FREQ_16_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_3;
		break;
	case AIC3X_FREQ_22_05_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = true;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_2;
		break;
	case AIC3X_FREQ_24_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_2;
		break;
	case AIC3X_FREQ_32_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_1_5;
		break;
	case AIC3X_FREQ_44_1_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = true;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_1;
		break;
	case AIC3X_FREQ_48_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = AIC3X_S_RATE_SEL_1;
		break;
	case AIC3X_FREQ_88_2_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = true;
		sampleRateControlBuffer.DoubleRate = true;
		sampleRateControlBuffer.SampleRateSel = SampleRateSel_1;
		break;
	case AIC3X_FREQ_96_KHZ:
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = true;
		sampleRateControlBuffer.SampleRateSel = SampleRateSel_1;
		break;
	default:
		/* default to 48 kHz */
		sampleRateControlBuffer.BaseRate_44_1_kHz = false;
		sampleRateControlBuffer.DoubleRate = false;
		sampleRateControlBuffer.SampleRateSel = SampleRateSel_1;
		break;
	}
	uint8_t dataPathReadBuffer;
	HAL_StatusTypeDef status;

	/* read data-path register */
	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_CODEC_DATAPATH_REG,
	I2C_MEMADD_SIZE_8BIT, &dataPathReadBuffer, sizeof(dataPathReadBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	uint8_t rateSelWriteBuffer;

	uint8_t dataPathWriteBuffer;
	uint8_t baseRateRegisterBits;
	uint8_t doubleRateRegisterBits;

	/* Set rateSel bits */
	rateSelWriteBuffer = (sampleRateControlBuffer.SampleRateSel<< 4)
			| (sampleRateControlBuffer.SampleRateSel);

	baseRateRegisterBits = sampleRateControlBuffer.BaseRate_44_1_kHz << 7;

	doubleRateRegisterBits = (sampleRateControlBuffer.DoubleRate << 6) | (sampleRateControlBuffer.DoubleRate << 5);


	/* Set the data-path bits */
	dataPathWriteBuffer |= dataPathReadBuffer
			& ~(AIC3X_BASE_RATE_MASK | AIC3X_RATE_DOUBLING_MASK);

	/* Set the base rate and rate doubling bits */
	dataPathWriteBuffer |= baseRateRegisterBits | doubleRateRegisterBits;

	/* Write the rateSel bits */
	status = HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_SAMPLE_RATE_SEL_REG, I2C_MEMADD_SIZE_8BIT, &rateSelWriteBuffer, 1U,
			10);

	if (status != HAL_OK) {
		return status;
	}

	/* Write the data-path bits */
	status = HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_CODEC_DATAPATH_REG, I2C_MEMADD_SIZE_8BIT, &dataPathWriteBuffer, 1U,
			10);

	if (status != HAL_OK) {
		return status;
	}

	/* Set the SampleRegMap if a valid pointer is given */
	if (pSampleRateControl != NULL) {
		*pSampleRateControl = sampleRateControlBuffer;
	}

	return HAL_OK;

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

	uint8_t pllQRegisterValue = pllRegisterReadBuffer
			>> 24& AIC3X_PLL_Q_MASK;
	uint8_t pllPRegisterValue = pllRegisterReadBuffer
			>> 24& AIC3X_PLL_P_MASK;
	uint8_t pllJRegisterValue = pllRegisterReadBuffer
			>> 16& AIC3X_PLL_J_MASK;
	uint16_t pllDRegisterValue = (uint16_t) pllRegisterReadBuffer
			& AIC3X_PLL_D_MASK;
	uint8_t pllRRegisterValue = overflowRegisterReadBuffer
			& AIC3X_PLL_R_MASK;

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

/**
 * @brief Reads the data path control and stores it in a given struct.
 *
 * Reads the data path control from the codec data path register and stores it
 * in a given struct.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pDataPathControl Pointer to a AIC3X_DataPathControl_TypeDef structure
 * 	that will be populated with the data path control.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_ReadDataPathControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DataPathControl_TypeDef *pDataPathControl) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pDataPathControl == NULL) {
		return HAL_ERROR;
	}

	uint8_t readBuffer;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_CODEC_DATAPATH_REG, I2C_MEMADD_SIZE_8BIT, &readBuffer,
			sizeof(readBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	/* Mask irrelevant bits for left and right data path */
	uint8_t leftDataPathBuffer = readBuffer & AIC3X_DATAPATH_LEFT_MASK;
	uint8_t rightDataPathBuffer = readBuffer & AIC3X_DATAPATH_RIGHT_MASK;

	pDataPathControl->LeftDACDataPath =
			(AIC3X_DataPath_TypeDef) leftDataPathBuffer >> 3;
	pDataPathControl->RightDACDataPath =
			(AIC3X_DataPath_TypeDef) rightDataPathBuffer >> 1;

	return HAL_OK;
}

/**
 * @brief Writes the data path control config to the AIC3X
 *
 * Sets the data path control by reading the data path control register,
 * defining the needed register values, then combining these with the
 * existing values and writing them back to the registers.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pDataPathControl Pointer to a AIC3X_DataPathControl_TypeDef structure
 * 	which defines the data path control. The values will be checked.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_WriteDataPathControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DataPathControl_TypeDef *pDataPathControl) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pDataPathControl == NULL) {
		return HAL_ERROR;
	}

	uint8_t readBuffer;
	HAL_StatusTypeDef status;

	/* read data-path register */
	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_CODEC_DATAPATH_REG, I2C_MEMADD_SIZE_8BIT, &readBuffer,
			sizeof(readBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	uint8_t writeBuffer = 0;

	/* Set existing bits for rate doubling and base rate */
	writeBuffer |= (readBuffer
			& ~(AIC3X_DATAPATH_LEFT_MASK || AIC3X_DATAPATH_RIGHT_MASK));

	/* Set the left data path */
	writeBuffer |= pDataPathControl->LeftDACDataPath << 3;

	/* Set the right data path */
	writeBuffer |= pDataPathControl->RightDACDataPath << 1;

	/* Write the data-path bits */
	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_CODEC_DATAPATH_REG, I2C_MEMADD_SIZE_8BIT, &writeBuffer, 1U, 10);
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
HAL_StatusTypeDef USER_AIC3X_ReadASDataIntfControl(AIC3X_HandleTypeDef *haic3x,
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
	AIC3X_ASD_INTF_CTRLA, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &readBuffer,
			3u, 10);

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
	pASDIntfCtrl->ClkDriveCtrl =
			(readBuffer & AIC3X_ASD_CLK_DRIVE_CTRL_MASK) >> 21;
	pASDIntfCtrl->Effect3D = (readBuffer & AIC3X_ASD_EFFECT_3D_MASK) >> 19;
	pASDIntfCtrl->TransferMode = (AIC3X_ASD_TransferMode_TypeDef) ((readBuffer
			& AIC3X_ASD_TRANSFER_MODE_MASK) >> 14);
	pASDIntfCtrl->WordLength = (AIC3X_ASD_WordLength_TypeDef) ((readBuffer
			& AIC3X_ASD_WORD_LENGTH_MASK) >> 12);
	pASDIntfCtrl->Bclk265ClockMode = (readBuffer
			& AIC3X_ASD_BCLK_265_CLOCK_MODE_MASK) >> 11;
	pASDIntfCtrl->DACResync = (readBuffer & AIC3X_ASD_DAC_RESYNC_MASK)
			>> 10;
	pASDIntfCtrl->ADCResync = (readBuffer & AIC3X_ASD_ADC_RESYNC_MASK) >> 9;
	pASDIntfCtrl->ReSyncMute = (readBuffer & AIC3X_ASD_RESYNC_MUTE_MASK)
			>> 8;
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
HAL_StatusTypeDef USER_AIC3X_WriteASDataIntfControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ASDIntCtrl_TypeDef *pASDIntfCtrl) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pASDIntfCtrl == NULL) {
		return HAL_ERROR;
	}

	uint32_t readBuffer;
	HAL_StatusTypeDef status;


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
	AIC3X_ASD_INTF_CTRLA, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &writeBuffer,
			3u, 10);

}

/**
 * @brief Reads the codec overflow status
 *
 * Reads the codec overflow status and stores it in a given struct. After the
 * values are read, the codec overflow register bits are reset to 0.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pCodecOverflow Pointer to a AIC3X_CodecOverflow_TypeDef structure
 * 	that will be populated with the codec overflow status.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_ReadCodecOverflowStatus(
		AIC3X_HandleTypeDef *haic3x,
		AIC3X_CodecOverflow_TypeDef *pCodecOverflow) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pCodecOverflow == NULL) {
		return HAL_ERROR;
	}

	uint8_t readBuffer;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_OVRF_STATUS_AND_PLLR_REG, I2C_MEMADD_SIZE_8BIT, &readBuffer,
			sizeof(readBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	pCodecOverflow->leftADCOverflow = readBuffer >> 8;
	pCodecOverflow->rightADCOverflow = readBuffer >> 7;
	pCodecOverflow->leftDACOverflow = readBuffer >> 6;
	pCodecOverflow->rightDACOverflow = readBuffer >> 5;

	return HAL_OK;
}

//TODO: Update documentation
/**
 * @brief Reads the codec digital filter
 *
 * Reads the codec digital filter and stores it in a given struct.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pCodecDFilt Pointer to a AIC3X_CodecDigitalFilter_TypeDef
 * 	structure that will be populated with the codec digital filter status.
 * @return Hal Status
 */

HAL_StatusTypeDef USER_AIC3X_ReadCodecDigitalFilter(
		AIC3X_HandleTypeDef *haic3x,
		AIC3X_CodecDigitalFilter_TypeDef *pCodecDFilt) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pCodecDFilt == NULL) {
		return HAL_ERROR;
	}

	uint8_t readBuffer;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
			AIC3X_CODEC_DFILT_CTRL, I2C_MEMADD_SIZE_8BIT, &readBuffer,
			sizeof(readBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	pCodecDFilt->leftAdcHpf = (AIC3X_DFiltHPF_TypeDef) ((readBuffer
			& AIC3X_CODEC_DFILT_HPF_LEFT_MASK) >> 6);
	pCodecDFilt->rightAdcHpf = (AIC3X_DFiltHPF_TypeDef) ((readBuffer
			& AIC3X_CODEC_DFILT_HPF_RIGHT_MASK) >> 4);
	pCodecDFilt->leftDacDFilt = (bool) ((readBuffer
			& AIC3X_CODEC_DFILT_DFILT_LEFT_MASK) >> 3);
	pCodecDFilt->leftDacDeEmph = (bool) ((readBuffer
			& AIC3X_CODEC_DFILT_DEEMPH_LEFT_MASK) >> 2);
	pCodecDFilt->rightDacDFilt = (bool) ((readBuffer
			& AIC3X_CODEC_DFILT_DFILT_RIGHT_MASK) >> 1);
	pCodecDFilt->rightDacDeEmph = (bool) ((readBuffer
			& AIC3X_CODEC_DFILT_DEEMPH_RIGHT_MASK));

	return HAL_OK;
}

/**
 * @brief Sets the codec digital filter
 *
 * Sets the codec digital filter by first defining the needed register values
 * and writing them to the registers.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pCodecDFilt Pointer to a AIC3X_CodecDigitalFilter_TypeDef structure
 * 	which defines the codec digital filter. The values will be checked.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_WriteCodecDigitalFilter(
		AIC3X_HandleTypeDef *haic3x,
		AIC3X_CodecDigitalFilter_TypeDef *pCodecDFilt) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pCodecDFilt == NULL) {
		return HAL_ERROR;
	}

	uint8_t writeBuffer = 0;

	writeBuffer |= pCodecDFilt->leftAdcHpf << 6;
	writeBuffer |= pCodecDFilt->rightAdcHpf << 4;
	writeBuffer |= pCodecDFilt->leftDacDFilt << 3;
	writeBuffer |= pCodecDFilt->leftDacDeEmph << 2;
	writeBuffer |= pCodecDFilt->rightDacDFilt << 1;
	writeBuffer |= pCodecDFilt->rightDacDeEmph;

	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
			AIC3X_CODEC_DFILT_CTRL, I2C_MEMADD_SIZE_8BIT, &writeBuffer,
			sizeof(writeBuffer), 10);
}
/**
 * @brief Reads the headset button press and stores it in a given struct.
 *
 * Reads the headset button press from the headset button press register
 * and stores it in a given struct.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pHeadsetBtnPress Pointer to a AIC3X_HeadsetBtnPress_TypeDef structure
 * 	that will be populated with the headset button press status.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_ReadHeadsetBtnPress(AIC3X_HandleTypeDef *haic3x,
		AIC3X_HeadsetBtnPress_TypeDef *pHeadsetBtnPress) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pHeadsetBtnPress == NULL) {
		return HAL_ERROR;
	}

	uint16_t readBuffer;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
	AIC3X_HEADSET_BTN_PRESS_REG, I2C_MEMADD_SIZE_8BIT, &readBuffer,
			sizeof(readBuffer), 10);

	if (status != HAL_OK) {
		return status;
	}

	pHeadsetBtnPress->enabled = (bool) ((readBuffer
			& AIC3X_HEADSET_BTN_PRESS_ENABLE_MASK) >> 15);
	pHeadsetBtnPress->HeadsetType = (AIC3X_HeadsetType_TypeDef) ((readBuffer
			& AIC3X_HEADSET_TYPE_MASK) >> 13);
	pHeadsetBtnPress->DebounceJackTime = (AIC3X_HeadsetDebounceJack_TypeDef) ((readBuffer
			& AIC3X_DEBOUNCE_JACK_TIME_MASK) >> 10);
	pHeadsetBtnPress->DebounceButtonTime = (AIC3X_HeadsetDebounceBtn_TypeDef) ((readBuffer
			& AIC3X_DEBOUNCE_BUTTON_TIME_MASK) >> 8);
	pHeadsetBtnPress->DriverCapacitiveCoupling = (bool) ((readBuffer & AIC3X_DRIVER_CAPACITIVE_COUPLING_MASK) >> 7);
	pHeadsetBtnPress->StereoOutputDriverConfigA = (bool) ((readBuffer & AIC3X_STEREO_OUTPUT_DRIVER_CONFIG_A_MASK) >> 6);
	pHeadsetBtnPress->HeadsetDetectionFlag = (bool) ((readBuffer & AIC3X_HEADSET_DETECTION_FLAG_MASK) >> 4);
	pHeadsetBtnPress->StereoOutputDriverConfigB = (bool) ((readBuffer & AIC3X_STEREO_OUTPUT_DRIVER_CONFIG_B_MASK) >> 3);

	return HAL_OK;
}

/**
 * @brief Sets the headset button press
 *
 * Sets the headset button press by first defining the needed register values
 * and writing them to the registers.
 *
 * @param haic3x Pointer to a AIC3X_HandleTypeDef structure that contains
 *  the configuration information for the specified AIC3X.
 * @param pHeadsetBtnPress Pointer to a AIC3X_HeadsetBtnPress_TypeDef structure
 * 	which defines the headset button press. The values will be checked.
 * @return Hal Status
 */
HAL_StatusTypeDef USER_AIC3X_WriteHeadsetBtnPress(
		AIC3X_HandleTypeDef *haic3x,
		AIC3X_HeadsetBtnPress_TypeDef *pHeadsetBtnPress) {

	if (haic3x == NULL) {
		return HAL_ERROR;
	}

	if (pHeadsetBtnPress == NULL) {
		return HAL_ERROR;
	}

	/* Check if the values are within the valid range */
	if (pHeadsetBtnPress->DebounceJackTime > 5 || ((pHeadsetBtnPress->StereoOutputDriverConfigA & pHeadsetBtnPress->StereoOutputDriverConfigB) == false)) {
		return HAL_ERROR;
	}


	uint16_t writeBuffer = 0;

	writeBuffer |= pHeadsetBtnPress->enabled << 15;
	writeBuffer |= pHeadsetBtnPress->DebounceJackTime << 10;
	writeBuffer |= pHeadsetBtnPress->DebounceBtnTime << 8;
	writeBuffer |= pHeadsetBtnPress->DriverCapacitiveCoupling << 7;
	writeBuffer |= pHeadsetBtnPress->StereoOutputDriverConfigA << 6;
	writeBuffer |= pHeadsetBtnPress->StereoOutputDriverConfigB << 3;

	return HAL_I2C_Mem_Write(haic3x->hi2c, (uint16_t) AIC3x_ADDRESS << 1,
			AIC3X_HEADSET_BTN_PRESS_REG, I2C_MEMADD_SIZE_8BIT, &writeBuffer,
			sizeof(writeBuffer), 10);
}
