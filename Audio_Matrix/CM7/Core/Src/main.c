/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "arm_math.h"

#include "aic3x.h"
#include "audio_status.h"
#include "audio_buffer.h"
#include "mdma.h"
#include "dsp.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */





#define ATOMIC_SECTION(code_block)  \
    do {                            \
        __disable_irq();            \
        code_block                  \
        __enable_irq();             \
    } while (0)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c2;

SAI_HandleTypeDef hsai_BlockA1;
SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_a;
DMA_HandleTypeDef hdma_sai1_b;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
AIC3X_HandleTypeDef aic3xHandle;

volatile uint32_t rxTransferCounter = 0;
volatile uint32_t txTransferCounter = 0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_DMA_Init(void);
static void MX_GPIO_Init(void);
static void MX_SAI1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void MDMA_RxXferBlockCpltCallback(MDMA_HandleTypeDef *_hdma);
void MDMA_TxXferBlockCpltCallback(MDMA_HandleTypeDef *_hdma);
void MDMA_RxErrorCallback(MDMA_HandleTypeDef *_hdma);
void MDMA_TxErrorCallback(MDMA_HandleTypeDef *_hdma);
HAL_StatusTypeDef codecSetup();


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
	int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
	/* Wait until CPU2 boots and enters in stop mode or timeout*/

	timeout = 0xFFFF;
	while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0))
		;
	if (timeout < 0) {
		Error_Handler();
	}
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
	/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
	 HSEM notification */
	/*HW semaphore Clock enable*/
	__HAL_RCC_HSEM_CLK_ENABLE();
	/*Take HSEM */
	HAL_HSEM_FastTake(HSEM_ID_0);
	/*Release HSEM in order to notify the CPU2(CM4)*/
	HAL_HSEM_Release(HSEM_ID_0, 0);
	/* wait until CPU2 wakes up from stop mode */
	timeout = 0xFFFF;
	while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0))
		;
	if (timeout < 0) {
		Error_Handler();
	}
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_GPIO_Init();
  MX_SAI1_Init();
  MX_I2C2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* Initialize the sai and audio buffers */
  ABUF_init();

  /* Initialize the MDMA */
  MDMA_init();
  MDMA_registerRxCallbacks(MDMA_RxXferBlockCpltCallback, MDMA_RxErrorCallback);
  MDMA_registerTxCallbacks(MDMA_TxXferBlockCpltCallback, MDMA_TxErrorCallback);

  /* Initialize the audio status */
  ASTAT_init(MDMA_hmdmaRxInstance, MDMA_hmdmaTxInstance);

  DSP_init();


  HAL_StatusTypeDef status;
  HAL_StatusTypeDef saiStatus;


	saiStatus = HAL_SAI_Init(&hsai_BlockA1);
	saiStatus = HAL_SAI_Init(&hsai_BlockB1);
	__HAL_SAI_ENABLE(&hsai_BlockA1);
	__HAL_SAI_ENABLE(&hsai_BlockB1);

	HAL_Delay(100);
	  HAL_StatusTypeDef MDMA_startRxTransfer(void);
	  HAL_StatusTypeDef MDMA_startTxTransfer(void);

	status = codecSetup();
	  if(HAL_OK != HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t*)ABUF_saiBufferPtrs[0].leftRxFirstHalf, SAI_BUFFER_LENGTH))
	  {
		Error_Handler();
	  }
	  if(HAL_OK != HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t*)ABUF_saiBufferPtrs[0].leftTxFirstHalf, SAI_BUFFER_LENGTH))
	  {
		Error_Handler();
	  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		ASTAT_rxXferRequestHandler();
		ASTAT_DspRequestHandler();
		ASTAT_TxXferRequestHandler();

		ASTAT_RxXferBlockCpltHandler();
		ASTAT_TxXferBlockCpltHandler();

		DSP_process();


	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 65;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1|RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.PLL2.PLL2M = 5;
  PeriphClkInitStruct.PLL2.PLL2N = 24;
  PeriphClkInitStruct.PLL2.PLL2P = 25;
  PeriphClkInitStruct.PLL2.PLL2Q = 24;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00000873;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_RX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MCK_OVERSAMPLING_DISABLE;
  hsai_BlockA1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.AudioMode = SAI_MODESLAVE_TX;
  hsai_BlockB1.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_16BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RESET_Pin */
  GPIO_InitStruct.Pin = RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//TODO Adress/Size aligment error, Read error
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
	CLEAR_BIT(ASTAT_saiStatus, SAI_STATUS_RX_HALF_PENDING);
	SET_BIT(ASTAT_saiStatus, SAI_STATUS_RX_HALF_CPLT);
	CLEAR_BIT(ASTAT_saiStatus, SAI_STATUS_RX_FULL_CPLT);
	SET_BIT(ASTAT_saiStatus, SAI_STATUS_RX_FULL_PENDING);

	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{
	CLEAR_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_HALF_PART);
	CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_HALF_PART);
	CLEAR_BIT(ASTAT_txStatus[i], AUDIO_STATUS_HALF_PART);
	}

}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
	CLEAR_BIT(ASTAT_saiStatus, SAI_STATUS_RX_FULL_PENDING);
	SET_BIT(ASTAT_saiStatus, SAI_STATUS_RX_FULL_CPLT);
	CLEAR_BIT(ASTAT_saiStatus, SAI_STATUS_RX_HALF_CPLT);
	SET_BIT(ASTAT_saiStatus, SAI_STATUS_RX_HALF_PENDING);

	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{
	CLEAR_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_FULL);
	CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_FULL);
	CLEAR_BIT(ASTAT_txStatus[i], AUDIO_STATUS_FULL);
	}

}

//void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
////	if(READ_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_CPLT) == false
////			&& READ_BIT(ASTAT_TxStatus, AUDIO_STATUS_R_HALF_CPLT) == false){
////		SET_BIT(memoryTransferError, SAI_TX_UNDERRUN_ERROR);
////		Error_Handler();
////	}
//	//CLEAR_BIT(ASTAT_TxStatus, AUDIO_STATUS_FULL);
//
//}

//void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai) {
////	if(READ_BIT(ASTAT_TxStatus, AUDIO_STATUS_L_CPLT) == false
////			&& READ_BIT(ASTAT_TxStatus, AUDIO_STATUS_R_CPLT) == false){
////		SET_BIT(memoryTransferError, SAI_TX_UNDERRUN_ERROR);
////		Error_Handler();
////	}
//	//CLEAR_BIT(ASTAT_TxStatus, AUDIO_STATUS_HALF_PART);
//}

void MDMA_RxXferBlockCpltCallback(MDMA_HandleTypeDef *_hdma)
{
	rxTransferCounter++;
	ASTAT_isRxTransferComplete = true;
}

void MDMA_TxXferBlockCpltCallback(MDMA_HandleTypeDef *_hdma)
{
	txTransferCounter++;
	ASTAT_isTxTransferComplete = true;
}


void MDMA_RxErrorCallback(MDMA_HandleTypeDef *_hdma){
	Error_Handler();
}

void MDMA_TxErrorCallback(MDMA_HandleTypeDef *_hdma){
	Error_Handler();
}

HAL_StatusTypeDef codecSetup(){
	HAL_StatusTypeDef status;

	aic3xHandle.hi2c = &hi2c2;
	aic3xHandle.rst_port = RESET_GPIO_Port;
	aic3xHandle.rst_pin = RESET_Pin;


	status = AIC3X_Init(&aic3xHandle);

	if (status != HAL_OK) {
		return status;
	}

	AIC3X_ASDIntCtrl_TypeDef asdIntCtrl = {
			.BclkOutput = false,
			.WclkOutput = false,
			.DoutTristate = true,
			.ClkDriveCtrl = false,
			.Effect3D = false,
			.TransferMode = AIC3X_ASD_I2S,
			.WordLength = AIC3X_ASD_16_BIT,
			.Bclk265ClockMode = false,
			.DACResync = false,
			.ADCResync = false,
			.ReSyncMute = false,
			.WordOffset = 0
	};


	status = AIC3X_WriteASDataIntfControl(&aic3xHandle, &asdIntCtrl);

	if (status != HAL_OK) {
		return status;
	}

//	  		AIC3X_PLLProgramming_TypeDef pllProgramming = {
//	  				.Enable = false,
//	  				.PllQ = 2,
//	  				.PllP = 1,
//	  				.PllJ = 1,
//	  				.PllD = 0,
//	  				.PllR = 8
//	  		};
//
//	  		status = USER_AIC3X_WritePllProgramming(&aic3xHandle, &pllProgramming);

	AIC3X_PGAGainControl_TypeDef pgaGainControl = {
			.LADCMuted = false,
			.LADCGain = 0,
			.RADCMuted = false,
			.RADCGain = 0
	};


	AIC3X_ADCMixControl_TypeDef adcMixControl = {
			.LeftADC_MIC2LGain = 0,
				.LeftADC_MIC2RGain = 0b1111,
				.RightADC_MIC2LGain = 0b1111,
				.RightADC_MIC2RGain = 0,
				.LeftADC_MIC1LPDifferential = false,
				.LeftADC_MIC1LPGain = 0b1111,
				.LeftADC_PowerUp = true,
				.LeftADC_PGASoftStepping = 0b00,
				.LeftADC_MIC1RPDifferential = false,
				.LeftADC_MIC1RPGain = 0b1111,
				.RightADC_MIC1RPDifferential = false,
				.RightADC_MIC1RPGain = 0b1111,
				.RightADC_PowerUp = true,
				.RightADC_PGASoftStepping = 0b00,
				.RightADC_MIC1LPDifferential = false,
				.RightADC_MIC1LPGain = 0b1111
	};

	status = AIC3X_WriteADCMixControl(&aic3xHandle, &adcMixControl);

	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_WritePGAGainControl(&aic3xHandle, &pgaGainControl);

	if (status != HAL_OK) {
		return status;
	}


	AIC3X_DataPathControl_TypeDef dataPathControl = {
			.LeftDACDataPath = AIC3X_DATAPATH_STRAIGHT,
			.RightDACDataPath = AIC3X_DATAPATH_STRAIGHT
	};

	status = AIC3X_WriteDataPathControl(&aic3xHandle, &dataPathControl);

	if (status != HAL_OK) {
		return status;
	}

	AIC3X_DACPowerControl_TypeDef dacPowerControl = {
			.LeftDACPoweredUp = true,
			.RightDACPoweredUp = true
	};

	status = AIC3X_WriteDACPowerControl(&aic3xHandle, &dacPowerControl);

	if (status != HAL_OK) {
		return status;
	}


	AIC3X_VolControl_TypeDef defaultVolControl = {
			.Muted = false,
			.Volume = 0,
	};

	status = AIC3X_WriteVolControl(&aic3xHandle,LDAC_VOL, &defaultVolControl);

	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_WriteVolControl(&aic3xHandle,RDAC_VOL, &defaultVolControl);

	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_WriteVolControl(&aic3xHandle,DACL1_2_LLOPM_VOL, &defaultVolControl);

	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_WriteVolControl(&aic3xHandle,DACR1_2_RLOPM_VOL, &defaultVolControl);

	if (status != HAL_OK) {
		return status;
	}

	AIC3X_OutputLevelControl_TypeDef defaultOutputLevelControl = {
			.Level = 0,
			.Muted = false,
			.PowerDownDriveControl = false,
			.VolumeControlStatus = false,
			.PowerControl = true
	};

	status = AIC3X_WriteOutputLevelControl(&aic3xHandle, LLOPM_CTRL, &defaultOutputLevelControl);

	if (status != HAL_OK) {
		return status;
	}

	status = AIC3X_WriteOutputLevelControl(&aic3xHandle, RLOPM_CTRL, &defaultOutputLevelControl);

	return status;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
		__BKPT();
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
