/**
 * @file user_aic3x.h
 *
 * @brief Driver for the AIC3X audio codec
 *
 * @author Dominik Grob
 *
 */

#ifndef USER_INC_USER_AIC3X_H_
#define USER_INC_USER_AIC3X_H_

#include "stm32h7xx_hal.h"
#include "stdbool.h"

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
 * @brief Sample rates the codec can operate on
 *
 */
typedef enum __AIC3X_SampleRate_TypeDef {
	AIC3X_FREQ_8_KHZ = 8000, /**< Frequency of 8 kHz */
	AIC3X_FREQ_11_025_KHZ = 11025,/**< Frequency of 11.025 kHz */
	AIC3X_FREQ_12_KHZ = 12000, /**< Frequency of 12 kHz */
	AIC3X_FREQ_16_KHZ = 16000, /**< Frequency of 16 kHz */
	AIC3X_FREQ_22_05_KHZ = 22050, /**< Frequency of 22.05 kHz */
	AIC3X_FREQ_24_KHZ = 24000, /**< Frequency of 24 kHz */
	AIC3X_FREQ_32_KHZ = 32000, /**< Frequency of 32 kHz */
	AIC3X_FREQ_44_1_KHZ = 44100, /**< Frequency of 44.1 kHz */
	AIC3X_FREQ_48_KHZ = 48000, /**< Frequency of 48 kHz */
	AIC3X_FREQ_88_2_KHZ = 88200, /**< Frequency of 88.2 kHz */
	AIC3X_FREQ_96_KHZ = 96000 /**< Frequency of 96 kHz */
} AIC3X_SampleRate_TypeDef;

/**
 * @brief Sample rate select division modifier values
 *
 */
typedef enum __AIC3X_SampleRateSel_TypeDef {
	AIC3X_S_RATE_SEL_1 = 0b0000, /**< Divides the sample rate by 1 */
	AIC3X_S_RATE_SEL_1_5 = 0b0001, /**< Divides the sample rate by 1.5 */
	AIC3X_S_RATE_SEL_2 = 0b0010, /**< Divides the sample rate by 2 */
	AIC3X_S_RATE_SEL_2_5 = 0b0011, /**< Divides the sample rate by 2.5 */
	AIC3X_S_RATE_SEL_3 = 0b0100, /**< Divides the sample rate by 3 */
	AIC3X_S_RATE_SEL_3_5 = 0b0101, /**< Divides the sample rate by 3.5 */
	AIC3X_S_RATE_SEL_4 = 0b0110, /**< Divides the sample rate by 4 */
	AIC3X_S_RATE_SEL_4_5 = 0b0111, /**< Divides the sample rate by 4.5 */
	AIC3X_S_RATE_SEL_5 = 0b1000, /**< Divides the sample rate by 5 */
	AIC3X_S_RATE_SEL_5_5 = 0b1001, /**< Divides the sample rate by 5.5 */
	AIC3X_S_RATE_SEL_6 = 0b1010, /**< Divides the sample rate by 6 */
} AIC3X_SampleRateSel_TypeDef;

/**
 * @brief Defines the sample rate of the codec
 *
 * Defines the sample rate of the codec and how it is generated.
 *
 */
typedef struct __AIC3X_SampleRateControl_Typedef {
	AIC3X_SampleRate_TypeDef SampleRate; /**< Sample rate in Hz */
	bool BaseRate_44_1_kHz; /**< Toggles the base sample rate between 48 kHz and 44.1 kHz */
	AIC3X_SampleRateSel_TypeDef SampleRateSel; /**< Defines the sample rate Select division factor */
	bool DoubleRate; /**< Sets the adc and dac to double rate sampling */
} AIC3X_SampleRateControl_Typedef;


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


/**
 * @brief Codec overflow values
 *
 * Defines the codec overflow values.
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
 * @brief Audio serial data interface control values
 *
 * Defines the audio serial data interface control values.
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
 * @brief Codec overflow values
 *
 * Defines the codec overflow values.
 *
 */
typedef struct __AIC3X_CodecOverflow_TypeDef {
	bool leftADCOverflow; /**< Left ADC overflow */
	bool rightADCOverflow; /**< Right ADC overflow */
	bool leftDACOverflow; /**< Left DAC overflow */
	bool rightDACOverflow; /**< Right DAC overflow */
} AIC3X_CodecOverflow_TypeDef;

/**
 * @brief Codec digital filter values
 *
 * Defines the codec digital filter values.
 *
 */
typedef enum __AIC3X_DFiltHPF_TypeDef {
	AIC3X_CODEC_HPF_DISABLE = 0b00, /**< Disable the high pass filter */
	AIC3X_CODEC_HPF_0_0045 = 0b01, /**< High pass filter with a -3db cut-off frequency of 0.0045 sample frequency */
	AIC3X_CODEC_HPF_0_0125 = 0b10, /**< High pass filter with a -3db cut-off frequency of 0.0125 sample frequency */
	AIC3X_CODEC_HPF_0_025 = 0b11 /**< High pass filter with a -3db cut-off frequency of 0.025 sample frequency */
} AIC3X_DFiltHPF_TypeDef;

/**
 * @brief Codec digital filter values
 *
 * Defines the codec digital filter values.
 *
 */
typedef struct __AIC3X_CodecDigitalFilter_TypeDef {
	AIC3X_DFiltHPF_TypeDef leftAdcHpf; /**< Left ADC -3db high pass filter */
	AIC3X_DFiltHPF_TypeDef rightAdcHpf; /**< Right ADC -3db high pass filter */
	bool leftDacDFilt; /**< Left DAC digital effects filter control */
	bool leftDacDeEmph; /**< Left DAC de-emphasis filter control */
	bool rightDacDFilt; /**< Right DAC digital effects filter control */
	bool rightDacDeEmph; /**< Right DAC de-emphasis filter control */
} AIC3X_CodecDigitalFilter_TypeDef;

/**
 * @brief Headset Type Detection Results
 *
 * Defines the headset type detected by the codec.
 *
 */
typedef enum __AIC3X_HeadsetType_TypeDef {
	AIC3X_HS_BTN_NO_HS = 0b00, /**< No headset connected */
	AIC3X_HS_BTN_NO_MIC = 0b01, /**< Headset connected without microphone */
	AIC3X_HS_BTN_IGNORE = 0b10, /**< Ignore this value*/
	AIC3X_HS_BTN_WITH_MIC = 0b11 /**< Headset connected with microphone */

} AIC3X_HeadsetType_TypeDef;

/**
 * @brief Headset Jack debounce time
 *
 * Defines the debounce time for the headset jack detection.
 *
 */
typedef enum __AIC3X_HeadsetDebounceJack_TypeDef {
	AIC3X_HS_DEBOUNCE_JACK_16 = 0b000, /**< Debounce time of 16ms */  /**< AIC3X_HS_DEBOUNCE_JACK_16 */
	AIC3X_HS_DEBOUNCE_JACK_32 = 0b001, /**< Debounce time of 32ms */  /**< AIC3X_HS_DEBOUNCE_JACK_32 */
	AIC3X_HS_DEBOUNCE_JACK_64 = 0b010, /**< Debounce time of 64ms */  /**< AIC3X_HS_DEBOUNCE_JACK_64 */
	AIC3X_HS_DEBOUNCE_JACK_128 = 0b011, /**< Debounce time of 128ms *//**< AIC3X_HS_DEBOUNCE_JACK_128 */
	AIC3X_HS_DEBOUNCE_JACK_256 = 0b100, /**< Debounce time of 256ms *//**< AIC3X_HS_DEBOUNCE_JACK_256 */
	AIC3X_HS_DEBOUNCE_JACK_512 = 0b101, /**< Debounce time of 512ms *//**< AIC3X_HS_DEBOUNCE_JACK_512 */
} AIC3X_HeadsetDebounceJack_TypeDef;

/**
 * @brief Headset Button debounce time
 *
 * Defines the debounce time for the headset button detection.
 *
 */
typedef enum __AIC3X_HeadsetDebounceBtn_TypeDef {
	AIC3X_HS_DEBOUNCE_BTN_0 = 0b00, /**< Debounce time of 0ms */
	AIC3X_HS_DEBOUNCE_BTN_8 = 0b01, /**< Debounce time of 8ms */
	AIC3X_HS_DEBOUNCE_BTN_16 = 0b10, /**< Debounce time of 16ms */
	AIC3X_HS_DEBOUNCE_BTN_32 = 0b11, /**< Debounce time of 32ms */
} AIC3X_HeadsetDebounceBtn_TypeDef;

/**
 * @brief Headset button press detection values
 *
 * Defines the headset button press detection values.
 *
 */
typedef struct __AIC3X_HeadsetBtnPress_TypeDef {
	bool enabled; /** Headset button press detection enabled */
	AIC3X_HeadsetType_TypeDef HeadsetType; /**< Headset type Detection Results (Read-Only) */
	AIC3X_HeadsetDebounceJack_TypeDef DebounceJackTime; /**< Headset Glitch Suppression Debounce Control for Jack Detection */
	AIC3X_HeadsetDebounceBtn_TypeDef DebounceBtnTime; /**< Headset Glitch Suppression Debounce Control for Button Press */
	bool DriverCapacitiveCoupling; /**< Headset driver capacitive coupling */
	bool StereoOutputDriverConfigA; /**< Stereo output driver configuration A: if a fully differential ouput configuration is used. Do not set if StereoDriverConfigB is set*/
	bool HeadsetDetectionFlag; /**< Headset detection flag (Read-Only) */
	bool StereoOutputDriverConfigB; /**< Stereo output driver configuration B: if a psuedodifferential ouput configuration is used. Do not set if StereoDriverConfigA is set*/
} AIC3X_HeadsetBtnPress_TypeDef;

/* I2C adress of the chip*/
#define AIC3x_ADDRESS 	0x18

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
/* MIC3 control registers */
#define MIC3LR_2_LADC_CTRL		17
#define MIC3LR_2_RADC_CTRL		18
/* Line1 Input control registers */
#define LINE1L_2_LADC_CTRL		19
#define LINE1R_2_LADC_CTRL		21
#define LINE1R_2_RADC_CTRL		22
#define LINE1L_2_RADC_CTRL		24
/* Line2 Input control registers */
#define LINE2L_2_LADC_CTRL		20
#define LINE2R_2_RADC_CTRL		23
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

/* Register values of the division factors for the codec sample rate */
#define SampleRateSel_1     0x11
#define SampleRateSel_1_5   0x22
#define SampleRateSel_2     0x33
#define SampleRateSel_2_5   0x44
#define SampleRateSel_3     0x55
#define SampleRateSel_3_5   0x66
#define SampleRateSel_4     0x77
#define SampleRateSel_4_5   0x88
#define SampleRateSel_5     0x99
#define SampleRateSel_5_5   0xAA
#define SampleRateSel_6     0xBB

HAL_StatusTypeDef AIC3X_Init(AIC3X_HandleTypeDef *haic3x);
HAL_StatusTypeDef AIC3X_HwReset(AIC3X_HandleTypeDef *haic3x);
HAL_StatusTypeDef AIC3X_SwReset(AIC3X_HandleTypeDef *haic3x);
HAL_StatusTypeDef USER_AIC3X_ReadSampleRateSel(AIC3X_HandleTypeDef *haic3x,
		AIC3X_SampleRateControl_Typedef *pSampleRateRegMap);
HAL_StatusTypeDef USER_AIC3X_WriteSampleRateSel(AIC3X_HandleTypeDef *haic3x,
		AIC3X_SampleRate_TypeDef sampleRate,
		AIC3X_SampleRateControl_Typedef *pSampleRateRegMap);
HAL_StatusTypeDef USER_AIC3X_ReadPllProgramming(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PLLProgramming_TypeDef *pPllProgramming);
HAL_StatusTypeDef USER_AIC3X_WritePllProgramming(AIC3X_HandleTypeDef *haic3x,
		AIC3X_PLLProgramming_TypeDef *pPllProgramming);
HAL_StatusTypeDef USER_AIC3X_ReadDataPathControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DataPathControl_TypeDef *pDataPathControl);
HAL_StatusTypeDef USER_AIC3X_WriteDataPathControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_DataPathControl_TypeDef *pDataPathControl);
HAL_StatusTypeDef USER_AIC3X_ReadASDataIntfControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ASDIntCtrl_TypeDef *pASDIntfCtrl);
HAL_StatusTypeDef USER_AIC3X_WriteASDataIntfControl(AIC3X_HandleTypeDef *haic3x,
		AIC3X_ASDIntCtrl_TypeDef *pASDIntfCtrl);
HAL_StatusTypeDef USER_AIC3X_ReadCodecOverflowStatus(
		AIC3X_HandleTypeDef *haic3x,
		AIC3X_CodecOverflow_TypeDef *pCodecOverflow);
HAL_StatusTypeDef USER_AIC3X_ReadCodecDigitalFilter(
		AIC3X_HandleTypeDef *haic3x,
		AIC3X_CodecDigitalFilter_TypeDef *pCodecDFilt);
HAL_StatusTypeDef USER_AIC3X_WriteCodecDigitalFilter(
		AIC3X_HandleTypeDef *haic3x,
		AIC3X_CodecDigitalFilter_TypeDef *pCodecDFilt);

#endif /* USER_INC_USER_AIC3X_H_ */
