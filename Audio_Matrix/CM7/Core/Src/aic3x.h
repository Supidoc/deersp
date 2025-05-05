/*
 * aic3x.h
 *
 *  Created on: Apr 21, 2025
 *      Author: dgrob
 */

#ifndef SRC_AIC3X_H_
#define SRC_AIC3X_H_

#include "stm32h7xx_hal.h"
#include "stdbool.h"

/* I2C adress of the chip*/
#define AIC3x_ADDRESS 	0b0011000

/**
 * @brief AIC3X handle Structure definition
 *
 */
typedef struct __AIC3X_HandleTypeDef {
	I2C_HandleTypeDef *hi2c; /**< I2C handle */
	GPIO_TypeDef *rst_port; /**< Reset pin GPIO port */
	uint16_t rst_pin; /**< Reset pin */
} AIC3X_HandleTypeDef;

/**
 * @brief Codec DataPath values
 *
 * Defines the codec DataPath values.
 *
 */
typedef enum __AIC3X_DataPath_TypeDef {
	AIC3X_DATAPATH_MUTED = 0b00, /**< Mute the data path */
	AIC3X_DATAPATH_STRAIGHT = 0b01, /**< Set the data path for the left channel to the left input or respectively the right to the right input */
	AIC3X_DATAPATH_CROSSED = 0b10, /**< Set the data path for the left channel to the right input or respectively the right to the left input */
	AIC3X_DATAPATH_MONO = 0b11, /**< Set the data path to mono mix of right and left input */
} AIC3X_DataPath_TypeDef;

/**
 * @brief Data path control values
 *
 * Defines the data path control values.
 *
 */
typedef struct __AIC3X_DataPathControl_TypeDef {
	AIC3X_DataPath_TypeDef LeftDACDataPath; /**< Data path for the left DAC */
	AIC3X_DataPath_TypeDef RightDACDataPath; /**< Data path for the right DAC */

} AIC3X_DataPathControl_TypeDef;

/**
 * @brief Audio serial data interface transfer mode values
 *
 * Defines the audio serial data interface transfer mode values.
 *
 */
typedef enum __AIC3X_ASD_TransferMode_TypeDef {
	AIC3X_ASD_I2S = 0b00, /**< I2S transfer mode */
	AIC3X_ASD_DSP = 0b01, /**< DSP transfer mode */
	AIC3X_ASD_RIGHT_JUSTIFIED = 0b10, /**< Right justified transfer mode */
	AIC3X_ASD_LEFT_JUSTIFIED = 0b11, /**< Left justified transfer mode */
} AIC3X_ASD_TransferMode_TypeDef;

/**
 * @brief Audio serial data interface word length values
 *
 * Defines the audio serial data interface word length values.
 *
 */
typedef enum __AIC3X_ASD_WordLength_TypeDef {
	AIC3X_ASD_16_BIT = 0b00, /**< 16 Bit Word length */
	AIC3X_ASD_20_BIT = 0b01, /**< 20 Bit Word length */
	AIC3X_ASD_24_BIT = 0b10, /**< 24 Bit Word length */
	AIC3X_ASD_32_BIT = 0b11, /**< 32 Bit Word length */
} AIC3X_ASD_WordLength_TypeDef;

/**
 * @brief Audio serial data interface control values
 *
 * Defines the audio serial data interface control values.
 *
 */
typedef struct __AIC3X_ASDIntCtrl_TypeDef {
	bool BclkOutput; /**< BCLK direction */
	bool WclkOutput; /**< WCLK direction */
	bool DoutTristate; /**< DOUT tristate */
	bool ClkDriveCtrl; /**< Set to still drive clock when powered down */
	bool Effect3D; /**< 3D effect */
	AIC3X_ASD_TransferMode_TypeDef TransferMode; /**< Transfer mode */
	AIC3X_ASD_WordLength_TypeDef WordLength; /**< Word length */
	bool Bclk265ClockMode; /**< BCLK 265 clock mode */
	bool DACResync; /**< DAC resync */
	bool ADCResync; /**<ADC resync */
	bool ReSyncMute; /**< Enable Soft-Mute on Resync */
	uint8_t WordOffset; /**< Word offset */
} AIC3X_ASDIntCtrl_TypeDef;

/**
 * @brief PLL programming values
 *
 * Defines the PLL programming values.
 *
 */
typedef struct __AIC3X_PLLProgramming_TypeDef {
	bool Enable; /**< Enable PLL programming */
	uint8_t PllQ; /**< PLL Q value which can be an integer value from 2 to 17 */
	uint8_t PllP; /**< PLL P value which can be an integer value from 1 to 8 */
	uint8_t PllJ; /**< PLL J value which can be an integer value from 1 to 63 */
	uint16_t PllD; /**< PLL D value which can be an integer value from 1 to 9999 */
	uint8_t PllR; /**< PLL R value which can be an integer value from 1 to 16 */

} AIC3X_PLLProgramming_TypeDef;

typedef struct __AIC3X_PGAGainControl_TypeDef {
	bool LADCMuted; /**< Left ADC muted */
	uint8_t LADCGain; /**< Left ADC gain with 0 = 0db, 1 = 0.5db with a max value of 59.5db of gain */
	bool RADCMuted; /**< Right ADC muted */
	uint8_t RADCGain; /**< Right ADC gain with 0 = 0db, 1 = 0.5db with a max value of 59.5db of gain */
} AIC3X_PGAGainControl_TypeDef;

typedef struct __AIC3X_ADCMixControl_TypeDef {
	uint8_t LeftADC_MIC2LGain; /**< MIC2L gain for left ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
	uint8_t LeftADC_MIC2RGain; /**< MIC2R gain for left ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
	uint8_t RightADC_MIC2LGain; /**< MIC2L gain for right ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
	uint8_t RightADC_MIC2RGain; /**< MIC2R gain for right ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
	bool LeftADC_MIC1LPDifferential; /**< Set MIC1LP to fully differential for left ADC */
	uint8_t LeftADC_MIC1LPGain; /**< MIC1LP gain for left ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
	bool LeftADC_PowerUp; /**< Power up  left ADC channel */
	uint8_t LeftADC_PGASoftStepping; /**< 0b00 = softstepping once per sample, 0b01 = softstepping once per 2 samples and 0b10-0b11 = softstepping disabled  */
	bool LeftADC_MIC1RPDifferential; /**< Set MIC1RP to fully differential for left ADC */
	uint8_t LeftADC_MIC1RPGain; /**< MIC1RP gain for left ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
	bool RightADC_MIC1RPDifferential; /**< Set MIC1RP to fully differential for right ADC */
	uint8_t RightADC_MIC1RPGain; /**< MIC1RP gain for right ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
	bool RightADC_PowerUp; /**< Power up right ADC channel */
	uint8_t RightADC_PGASoftStepping; /**< 0b00 = softstepping once per sample, 0b01 = softstepping once per 2 samples and 0b10-0b11 = softstepping disabled  */
	bool RightADC_MIC1LPDifferential; /**< Set MIC1LP to fully differential for right ADC */
	uint8_t RightADC_MIC1LPGain; /**< MIC1LP gain for right ADC with 0 = 0db, 1 = -1.5db with a max value of -12db of gain and 0b1111 for not connected to adc*/
} AIC3X_ADCMixControl_TypeDef;

typedef struct __AIC3X_DACPowerControl_TypeDef {
	bool LeftDACPoweredUp; /**< Left DAC powered up */
	bool RightDACPoweredUp; /**< Right DAC powered up */
} AIC3X_DACPowerControl_TypeDef;

typedef struct __AIC3X_VolControl_TypeDef {
	bool Muted; /**< muted */
	uint8_t Volume; /**< volume control, see datasheet for values */
} AIC3X_VolControl_TypeDef;

typedef struct __AIC3X_OutputLevelControl_TypeDef {
	uint8_t Level; /**< level in dB with values from 0 to 9 */
	bool Muted; /**< Output muted */
	bool PowerDownDriveControl; /**< Weakly driven when false and high impedance when true */
	bool VolumeControlStatus; /**< programmed gains applied when true and not applied when false */
	bool PowerControl
/**< Fully Powered up when true and powered down when false */
} AIC3X_OutputLevelControl_TypeDef;

HAL_StatusTypeDef AIC3X_Init(AIC3X_HandleTypeDef *haic3x);

HAL_StatusTypeDef AIC3X_HwReset(AIC3X_HandleTypeDef *haic3x);
HAL_StatusTypeDef AIC3X_SwReset(AIC3X_HandleTypeDef *haic3x);

HAL_StatusTypeDef AIC3X_ReadData(AIC3X_HandleTypeDef *haic3x, uint8_t reg,
		uint8_t *data, uint8_t dataSize);
HAL_StatusTypeDef AIC3X_WriteData(AIC3X_HandleTypeDef *haic3x, uint8_t reg,
		uint8_t *data, uint8_t dataSize);

HAL_StatusTypeDef AIC3X_WriteDataPathControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DataPathControl_TypeDef *pDataPathControl);
HAL_StatusTypeDef AIC3X_WriteASDataIntfControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ASDIntCtrl_TypeDef *pASDIntfCtrl);

HAL_StatusTypeDef USER_AIC3X_ReadPllProgramming(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PLLProgramming_TypeDef *pPllProgramming);
HAL_StatusTypeDef USER_AIC3X_WritePllProgramming(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PLLProgramming_TypeDef *pPllProgramming);

HAL_StatusTypeDef AIC3X_ReadPGAGainControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PGAGainControl_TypeDef *pPGAGainControl);
HAL_StatusTypeDef AIC3X_WritePGAGainControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PGAGainControl_TypeDef *pPGAGainControl);

HAL_StatusTypeDef AIC3X_ReadPGAControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ADCMixControl_TypeDef *pPGAControl);
HAL_StatusTypeDef AIC3X_ReadADCMixControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ADCMixControl_TypeDef *pPGAControl);

HAL_StatusTypeDef AIC3X_WriteADCMixControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ADCMixControl_TypeDef *pPGAControl);
HAL_StatusTypeDef AIC3X_ReadDACPowerControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DACPowerControl_TypeDef *pDACPowerControl);

HAL_StatusTypeDef AIC3X_WriteDACPowerControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DACPowerControl_TypeDef *pDACPowerControl);

HAL_StatusTypeDef AIC3X_ReadVolControl(AIC3X_HandleTypeDef *haic3x, uint8_t reg,
		AIC3X_VolControl_TypeDef *pVolControl);
HAL_StatusTypeDef AIC3X_WriteVolControl(AIC3X_HandleTypeDef *haic3x,
		uint8_t reg, AIC3X_VolControl_TypeDef *pVolControl);

HAL_StatusTypeDef AIC3X_ReadOutputLevelControl(AIC3X_HandleTypeDef *haic3x,
		uint8_t reg, AIC3X_OutputLevelControl_TypeDef *pOutputLevelControl);
HAL_StatusTypeDef AIC3X_WriteOutputLevelControl(AIC3X_HandleTypeDef *haic3x,
		uint8_t reg, AIC3X_OutputLevelControl_TypeDef *pOutputLevelControl);

/* Page select register */
#define AIC3X_PAGE_SELECT		0
/* Software reset register */
#define AIC3X_RESET			1
/* Codec Sample rate select register */
#define AIC3X_SAMPLE_RATE_SEL_REG	2
/* PLL progrramming register A */
#define AIC3X_PLL_PROGA_REG		3
/* PLL progrramming register B */
#define AIC3X_PLL_PROGB_REG		4
/* PLL progrramming register C */
#define AIC3X_PLL_PROGC_REG		5
/* PLL progrramming register D */
#define AIC3X_PLL_PROGD_REG		6
/* Codec datapath setup register */
#define AIC3X_CODEC_DATAPATH_REG	7
/* Audio serial data interface control register A */
#define AIC3X_ASD_INTF_CTRLA		8
/* Audio serial data interface control register B */
#define AIC3X_ASD_INTF_CTRLB		9
/* Audio serial data interface control register C */
#define AIC3X_ASD_INTF_CTRLC		10
/* Audio overflow status and PLL R value programming register */
#define AIC3X_OVRF_STATUS_AND_PLLR_REG	11
/* Audio codec digital filter control register */
#define AIC3X_CODEC_DFILT_CTRL		12
/* Headset/button press detection register */
#define AIC3X_HEADSET_DETECT_CTRL_A	13
#define AIC3X_HEADSET_DETECT_CTRL_B	14
/* ADC PGA Gain control registers */
#define LADC_VOL			15
#define RADC_VOL			16
/* MIC2 control registers */
#define MIC2LR_LADC_CTRL		17
#define MIC2LR_RADC_CTRL		18
/* Line1 Input control registers */
#define MIC1L_LADC_CTRL		19
#define MIC1R_LADC_CTRL		21
#define MIC1R_RADC_CTRL		22
#define MIC1L_RADC_CTRL		24
/* Line2 Input control registers */
#define MIC2L_LADC_CTRL		20
#define MIC2R_RADC_CTRL		23
/* MICBIAS Control Register */
#define MICBIAS_CTRL			25

/* AGC Control Registers A, B, C */
#define LAGC_CTRL_A			26
#define LAGC_CTRL_B			27
#define LAGC_CTRL_C			28
#define RAGC_CTRL_A			29
#define RAGC_CTRL_B			30
#define RAGC_CTRL_C			31

/* DAC Power and Left High Power Output control registers */
#define DAC_PWR				37
#define HPLCOM_CFG			37
/* Right High Power Output control registers */
#define HPRCOM_CFG			38
/* High Power Output Stage Control Register */
#define HPOUT_SC			40
/* DAC Output Switching control registers */
#define DAC_LINE_MUX			41
/* High Power Output Driver Pop Reduction registers */
#define HPOUT_POP_REDUCTION		42
/* DAC Digital control registers */
#define LDAC_VOL			43
#define RDAC_VOL			44
/* Left High Power Output control registers */
#define LINE2L_2_HPLOUT_VOL		45
#define PGAL_2_HPLOUT_VOL		46
#define DACL1_2_HPLOUT_VOL		47
#define LINE2R_2_HPLOUT_VOL		48
#define PGAR_2_HPLOUT_VOL		49
#define DACR1_2_HPLOUT_VOL		50
#define HPLOUT_CTRL			51
/* Left High Power COM control registers */
#define LINE2L_2_HPLCOM_VOL		52
#define PGAL_2_HPLCOM_VOL		53
#define DACL1_2_HPLCOM_VOL		54
#define LINE2R_2_HPLCOM_VOL		55
#define PGAR_2_HPLCOM_VOL		56
#define DACR1_2_HPLCOM_VOL		57
#define HPLCOM_CTRL			58
/* Right High Power Output control registers */
#define LINE2L_2_HPROUT_VOL		59
#define PGAL_2_HPROUT_VOL		60
#define DACL1_2_HPROUT_VOL		61
#define LINE2R_2_HPROUT_VOL		62
#define PGAR_2_HPROUT_VOL		63
#define DACR1_2_HPROUT_VOL		64
#define HPROUT_CTRL			65
/* Right High Power COM control registers */
#define LINE2L_2_HPRCOM_VOL		66
#define PGAL_2_HPRCOM_VOL		67
#define DACL1_2_HPRCOM_VOL		68
#define LINE2R_2_HPRCOM_VOL		69
#define PGAR_2_HPRCOM_VOL		70
#define DACR1_2_HPRCOM_VOL		71
#define HPRCOM_CTRL			72
/* Mono Line Output Plus/Minus control registers */
#define LINE2L_2_MONOLOPM_VOL		73
#define PGAL_2_MONOLOPM_VOL		74
#define DACL1_2_MONOLOPM_VOL		75
#define LINE2R_2_MONOLOPM_VOL		76
#define PGAR_2_MONOLOPM_VOL		77
#define DACR1_2_MONOLOPM_VOL		78
#define MONOLOPM_CTRL			79
/* Class-D speaker driver on tlv320aic3007 */
#define CLASSD_CTRL			73
/* Left Line Output Plus/Minus control registers */
#define LINE2L_2_LLOPM_VOL		80
#define PGAL_2_LLOPM_VOL		81
#define DACL1_2_LLOPM_VOL		82
#define LINE2R_2_LLOPM_VOL		83
#define PGAR_2_LLOPM_VOL		84
#define DACR1_2_LLOPM_VOL		85
#define LLOPM_CTRL			86
/* Right Line Output Plus/Minus control registers */
#define LINE2L_2_RLOPM_VOL		87
#define PGAL_2_RLOPM_VOL		88
#define DACL1_2_RLOPM_VOL		89
#define LINE2R_2_RLOPM_VOL		90
#define PGAR_2_RLOPM_VOL		91
#define DACR1_2_RLOPM_VOL		92
#define RLOPM_CTRL			93
/* GPIO/IRQ registers */
#define AIC3X_STICKY_IRQ_FLAGS_REG	96
#define AIC3X_RT_IRQ_FLAGS_REG		97
#define AIC3X_GPIO1_REG			98
#define AIC3X_GPIO2_REG			99
#define AIC3X_GPIOA_REG			100
#define AIC3X_GPIOB_REG			101
/* Clock generation control register */
#define AIC3X_CLKGEN_CTRL_REG		102
/* New AGC registers */
#define LAGCN_ATTACK			103
#define LAGCN_DECAY			104
#define RAGCN_ATTACK			105
#define RAGCN_DECAY			106
/* New Programmable ADC Digital Path and I2C Bus Condition Register */
#define NEW_ADC_DIGITALPATH		107
/* Passive Analog Signal Bypass Selection During Powerdown Register */
#define PASSIVE_BYPASS			108
/* DAC Quiescent Current Adjustment Register */
#define DAC_ICC_ADJ			109

/* Page select register bits */
#define PAGE0_SELECT		0
#define PAGE1_SELECT		1

/* Audio serial data interface control register A bits */
#define BIT_CLK_MASTER          0x80
#define WORD_CLK_MASTER         0x40
#define DOUT_TRISTATE		0x20

#endif /* SRC_AIC3201_H_ */
