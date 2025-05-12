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
#define AUDIO_CHANNEL_COUNT 2
#define AUDIO_BUFFER_LENGTH 256
#define AUDIO_SAMPLE_SIZE sizeof(uint16_t)
#define AUDIO_BUFFER_SIZE AUDIO_BUFFER_LENGTH*AUDIO_SAMPLE_SIZE
#define AUDIO_BUFFER_LENGTH_HALF AUDIO_BUFFER_LENGTH/2
#define AUDIO_BUFFER_SIZE_HALF AUDIO_BUFFER_SIZE/2

#define SAI_BUFFER_LENGTH AUDIO_BUFFER_LENGTH*AUDIO_CHANNEL_COUNT
#define SAI_BUFFER_SIZE SAI_BUFFER_LENGTH*AUDIO_SAMPLE_SIZE
#define SAI_BUFFER_LENGTH_HALF SAI_BUFFER_LENGTH/2

#define AUDIO_STATUS_L_HALF_PENDING 0x1
#define AUDIO_STATUS_L_HALF_CPLT 0x2
#define AUDIO_STATUS_R_HALF_PENDING 0x4
#define AUDIO_STATUS_R_HALF_CPLT 0x8
#define AUDIO_STATUS_HALF_PART 0xF
#define AUDIO_STATUS_L_PENDING 0x10
#define AUDIO_STATUS_L_CPLT 0x20
#define AUDIO_STATUS_R_PENDING 0x40
#define AUDIO_STATUS_R_CPLT 0x80
#define AUDIO_STATUS_FULL 0xF0

#define SAI_STATUS_RX_HALF_PENDING 0x1
#define SAI_STATUS_RX_HALF_CPLT 0x2
#define SAI_STATUS_RX_FULL_PENDING 0x4
#define SAI_STATUS_RX_FULL_CPLT 0x8
#define SAI_STATUS_TX_HALF_PENDING 0x10
#define SAI_STATUS_TX_HALF_CPLT 0x20
#define SAI_STATUS_TX_FULL_PENDING 0x40
#define SAI_STATUS_TX_FULL_CPLT 0x80

#define SAI_TX_UNDERRUN_ERROR 0x1


#define NUM_TAPS              29

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

MDMA_HandleTypeDef hmdma_mdma_channel0_sw_0;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_1;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_2;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_3;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_4;

MDMA_HandleTypeDef hmdma_mdma_channel2_sw_0;
__attribute__ ((aligned (8)))
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_1;
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_2;
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_3;
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_4;
/* USER CODE BEGIN PV */
AIC3X_HandleTypeDef aic3xHandle;


__attribute__ ((aligned (32)))
volatile uint16_t sai_buffer_rx[SAI_BUFFER_LENGTH];
__attribute__ ((aligned (8)))
volatile uint16_t sai_buffer_tx[SAI_BUFFER_LENGTH];

__attribute__((section(".audiobuffer"), aligned(8), used))
volatile uint16_t audio_buffer_rx_ch1_l[AUDIO_BUFFER_LENGTH];

__attribute__((section(".audiobuffer"), aligned(8), used))
volatile uint16_t audio_buffer_rx_ch1_r[AUDIO_BUFFER_LENGTH];

__attribute__((section(".audiobuffer"), aligned(8), used))
volatile uint16_t audio_buffer_tx_ch1_l[AUDIO_BUFFER_LENGTH];

__attribute__((section(".audiobuffer"), aligned(8), used))
volatile uint16_t audio_buffer_tx_ch1_r[AUDIO_BUFFER_LENGTH];

volatile uint8_t audio_rx_status = 0;
volatile uint8_t audio_dsp_status = 0;
volatile uint8_t audio_tx_status = 0;
volatile uint8_t sai_status = 0;

volatile uint8_t memoryTransferError = 0;

volatile bool startUpRx = true;
volatile bool startUpTx = true;

volatile uint32_t transfercounter = 0;

bool halfComplete = false;

static q15_t firStateQ15[AUDIO_BUFFER_LENGTH_HALF + NUM_TAPS - 1];

const q15_t firCoeffsQ15[NUM_TAPS] = {
  -0.0018225230f, -0.0015879294f, +0.0000000000f, +0.0036977508f, +0.0080754303f, +0.0085302217f, -0.0000000000f, -0.0173976984f,
  -0.0341458607f, -0.0333591565f, +0.0000000000f, +0.0676308395f, +0.1522061835f, +0.2229246956f, +0.2504960933f, +0.2229246956f,
  +0.1522061835f, +0.0676308395f, +0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, +0.0085302217f,
  +0.0080754303f, +0.0036977508f, +0.0000000000f, -0.0015879294f, -0.0018225230f
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_DMA_Init(void);
static void MX_GPIO_Init(void);
static void MX_MDMA_Init(void);
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
  MX_MDMA_Init();
  MX_SAI1_Init();
  MX_I2C2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  arm_fir_instance_q15 S;
  //arm_status status;

  //arm_fir_fast_q15(&S, NUM_TAPS, firCoeffsQ15, firStateQ15, AUDIO_BUFFER_LENGTH_HALF);

  HAL_StatusTypeDef status;
  HAL_StatusTypeDef saiStatus;

	saiStatus = HAL_SAI_Init(&hsai_BlockA1);
	saiStatus = HAL_SAI_Init(&hsai_BlockB1);
	__HAL_SAI_ENABLE(&hsai_BlockA1);
	__HAL_SAI_ENABLE(&hsai_BlockB1);
	SET_BIT(audio_rx_status, AUDIO_STATUS_L_HALF_PENDING);

	HAL_Delay(1000);

	status = codecSetup();
	  if(HAL_OK != HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t*)sai_buffer_rx, sizeof(sai_buffer_tx)/sizeof(uint16_t)))
	  {
		Error_Handler();
	  }
	  if(HAL_OK != HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t*)sai_buffer_tx, sizeof(sai_buffer_tx)/sizeof(uint16_t)))
	  {
		Error_Handler();
	  }

  /* USER CODE END 2 */

  /* Initialize User push-button without interrupt mode. */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		//Check for completed SAI RX and trigger tranfers from sai to audio buffers
		__disable_irq();
		if (READ_BIT(sai_status,
				SAI_STATUS_RX_HALF_CPLT) && READ_BIT(audio_rx_status, AUDIO_STATUS_L_HALF_PENDING) == false
				&& READ_BIT(audio_rx_status, AUDIO_STATUS_L_HALF_CPLT) == false)
		{
			//TODO Check if previous TX was completed
			__enable_irq();

			SET_BIT(audio_rx_status, AUDIO_STATUS_L_HALF_PENDING);
			if (startUpRx)
			{
				startUpRx = false;
				HAL_MDMA_Start_IT(&hmdma_mdma_channel0_sw_0,
						(uint32_t) sai_buffer_rx,
						(uint32_t) audio_buffer_rx_ch1_l, AUDIO_BUFFER_SIZE_HALF,
						1);
			}
			else
			{
				if (READ_BIT(audio_tx_status, AUDIO_STATUS_L_CPLT) == false
						&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_CPLT)
								== false)
				{
					__BKPT();
				}
				HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);

			}

		}
		else if (READ_BIT(audio_rx_status,
				AUDIO_STATUS_L_HALF_CPLT) && READ_BIT(audio_rx_status, AUDIO_STATUS_R_HALF_PENDING) == false
				&& READ_BIT(audio_rx_status, AUDIO_STATUS_R_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_rx_status, AUDIO_STATUS_R_HALF_PENDING);

			HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);
		}
		else if (READ_BIT(sai_status,
				SAI_STATUS_RX_FULL_CPLT) && READ_BIT(audio_rx_status, AUDIO_STATUS_L_PENDING) == false
				&& READ_BIT(audio_rx_status, AUDIO_STATUS_L_CPLT) == false)
		{
			//TODO Check if previous TX was completed
			__enable_irq();

			if (READ_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_CPLT) == false
					&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_CPLT)
							== false)
			{
				__BKPT();
			}
			SET_BIT(audio_rx_status, AUDIO_STATUS_L_PENDING);

			HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);
		}
		else if (READ_BIT(audio_rx_status,
				AUDIO_STATUS_L_CPLT) && READ_BIT(audio_rx_status, AUDIO_STATUS_R_PENDING) == false
				&& READ_BIT(audio_rx_status, AUDIO_STATUS_R_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_rx_status, AUDIO_STATUS_R_PENDING);

			HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);
		}
		else
		{
			__enable_irq();
		}

		//Check for completed audio rx transfers and run dsp

		__disable_irq();
		if (READ_BIT(audio_rx_status,
				AUDIO_STATUS_L_HALF_CPLT) && READ_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_PENDING) == false
				&& READ_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_CPLT) == false)
		{
			__enable_irq();
			SET_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_PENDING);
			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_CPLT);

			//Do DSP of left channel first half here
			memcpy((void*) audio_buffer_rx_ch1_l,
					(void*) audio_buffer_tx_ch1_l, AUDIO_BUFFER_LENGTH_HALF * AUDIO_SAMPLE_SIZE);

			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_PENDING);
			SET_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_CPLT);
		}
		else if (READ_BIT(audio_rx_status,
				AUDIO_STATUS_R_HALF_CPLT) && READ_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_PENDING) == false
				&& READ_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_PENDING);
			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_CPLT);

			//Do DSP of right channel first half here
			memcpy((void*) audio_buffer_rx_ch1_r,
					(void*) audio_buffer_tx_ch1_r, AUDIO_BUFFER_LENGTH_HALF * AUDIO_SAMPLE_SIZE);


		}
		else if (READ_BIT(audio_rx_status,
				AUDIO_STATUS_L_CPLT) && READ_BIT(audio_dsp_status, AUDIO_STATUS_L_PENDING) == false
				&& READ_BIT(audio_dsp_status, AUDIO_STATUS_L_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_dsp_status, AUDIO_STATUS_L_PENDING);
			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_L_CPLT);

			//Do DSP of left channel second half here
			memcpy((void*) audio_buffer_rx_ch1_l + AUDIO_BUFFER_LENGTH_HALF,
					(void*) audio_buffer_tx_ch1_l + AUDIO_BUFFER_LENGTH_HALF, AUDIO_BUFFER_LENGTH_HALF * AUDIO_SAMPLE_SIZE);

			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_L_PENDING);
			SET_BIT(audio_dsp_status, AUDIO_STATUS_L_CPLT);
		}
		else if (READ_BIT(audio_rx_status,
				AUDIO_STATUS_R_CPLT) && READ_BIT(audio_dsp_status, AUDIO_STATUS_R_PENDING) == false
				&& READ_BIT(audio_dsp_status, AUDIO_STATUS_R_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_dsp_status, AUDIO_STATUS_R_PENDING);
			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_R_CPLT);

			//Do DSP of left channel second half here
			memcpy((void*) audio_buffer_rx_ch1_r + AUDIO_BUFFER_LENGTH_HALF,
					(void*) audio_buffer_tx_ch1_r + AUDIO_BUFFER_LENGTH_HALF, AUDIO_BUFFER_LENGTH_HALF * AUDIO_SAMPLE_SIZE);

			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_R_PENDING);
			SET_BIT(audio_dsp_status, AUDIO_STATUS_R_CPLT);
		}
		else
		{
			__enable_irq();
		}



		//check for completed dsp and trigger tranfers from audio to sai buffer
		__disable_irq();
		if (READ_BIT(audio_dsp_status,
				AUDIO_STATUS_L_HALF_CPLT) && READ_BIT(audio_rx_status, AUDIO_STATUS_R_HALF_CPLT)
				&& READ_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_PENDING) == false
				&& READ_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_PENDING);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_CPLT);
			if (startUpTx)
			{
				startUpTx = false;
				HAL_MDMA_Start_IT(&hmdma_mdma_channel2_sw_0,
						(uint32_t) audio_buffer_tx_ch1_l,
						(uint32_t) sai_buffer_tx, AUDIO_BUFFER_SIZE_HALF,
						1);
			}
			else{
				HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel2_sw_0);
			}
		}
		else if (READ_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_CPLT) &&
		READ_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_CPLT)
		&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_PENDING) == false
		&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_PENDING);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_CPLT);
			HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel2_sw_0);
		}
		else if (READ_BIT(audio_dsp_status, AUDIO_STATUS_L_CPLT) &&
		READ_BIT(audio_rx_status, AUDIO_STATUS_R_CPLT)
		&& READ_BIT(audio_tx_status, AUDIO_STATUS_L_PENDING) == false
		&& READ_BIT(audio_tx_status, AUDIO_STATUS_L_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_tx_status, AUDIO_STATUS_L_PENDING);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_L_CPLT);
			HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel2_sw_0);
		}
		else if (READ_BIT(audio_dsp_status, AUDIO_STATUS_R_CPLT) &&
		READ_BIT(audio_tx_status, AUDIO_STATUS_L_CPLT)
		&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_PENDING) == false
		&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(audio_tx_status, AUDIO_STATUS_R_PENDING);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_R_CPLT);
			HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel2_sw_0);
		}
		else
		{
			__enable_irq();
		}


		if(READ_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_PENDING)){



			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_PENDING);
			SET_BIT(audio_dsp_status, AUDIO_STATUS_L_HALF_CPLT);
		}
		else if(READ_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_PENDING)){

			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_PENDING);
			SET_BIT(audio_dsp_status, AUDIO_STATUS_R_HALF_CPLT);
		}
		else if(READ_BIT(audio_dsp_status, AUDIO_STATUS_L_PENDING)){

			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_L_PENDING);
			SET_BIT(audio_dsp_status, AUDIO_STATUS_L_CPLT);
		}
		else if(READ_BIT(audio_dsp_status, AUDIO_STATUS_R_PENDING)){

			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_R_PENDING);
			SET_BIT(audio_dsp_status, AUDIO_STATUS_R_CPLT);
		}
//		if(BSP_PB_GetState(BUTTON_USER) == GPIO_PIN_SET){
//		halfComplete = halfComplete==false;
//
//		if(halfComplete){
//
//			CLEAR_BIT(sai_status, SAI_STATUS_RX_HALF_PENDING);
//			SET_BIT(sai_status, SAI_STATUS_RX_HALF_CPLT);
//			CLEAR_BIT(sai_status, SAI_STATUS_RX_FULL_CPLT);
//			SET_BIT(sai_status, SAI_STATUS_RX_FULL_PENDING);
//
//			CLEAR_BIT(audio_rx_status, AUDIO_STATUS_HALF_PART);
//			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_HALF_PART);
//			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_HALF_PART);
//
//		}else
//		{
//
//			CLEAR_BIT(sai_status, SAI_STATUS_RX_FULL_PENDING);
//			SET_BIT(sai_status, SAI_STATUS_RX_FULL_CPLT);
//			CLEAR_BIT(sai_status, SAI_STATUS_RX_HALF_CPLT);
//			SET_BIT(sai_status, SAI_STATUS_RX_HALF_PENDING);
//
//			CLEAR_BIT(audio_rx_status, AUDIO_STATUS_FULL);
//			CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_FULL);
//			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_FULL);
//
//		}
//		HAL_Delay(2000);
//		HAL_Delay(1);
//		}

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
  * Enable MDMA controller clock
  * Configure MDMA for global transfers
  *   hmdma_mdma_channel0_sw_0
  *   node_mdma_channel0_sw_1
  *   node_mdma_channel0_sw_2
  *   node_mdma_channel0_sw_3
  *   node_mdma_channel0_sw_4
  *   hmdma_mdma_channel2_sw_0
  *   node_mdma_channel2_sw_1
  *   node_mdma_channel2_sw_2
  *   node_mdma_channel2_sw_3
  *   node_mdma_channel2_sw_4
  */
static void MX_MDMA_Init(void)
{

  /* MDMA controller clock enable */
  __HAL_RCC_MDMA_CLK_ENABLE();
  /* Local variables */
  MDMA_LinkNodeConfTypeDef nodeConfig;

  /* Configure MDMA channel MDMA_Channel0 */
  /* Configure MDMA request hmdma_mdma_channel0_sw_0 on MDMA_Channel0 */
  hmdma_mdma_channel0_sw_0.Instance = MDMA_Channel0;
  hmdma_mdma_channel0_sw_0.Init.Request = MDMA_REQUEST_SW;
  hmdma_mdma_channel0_sw_0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  hmdma_mdma_channel0_sw_0.Init.Priority = MDMA_PRIORITY_HIGH;
  hmdma_mdma_channel0_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdma_mdma_channel0_sw_0.Init.SourceInc = MDMA_SRC_INC_WORD;
  hmdma_mdma_channel0_sw_0.Init.DestinationInc = MDMA_DEST_INC_HALFWORD;
  hmdma_mdma_channel0_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  hmdma_mdma_channel0_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  hmdma_mdma_channel0_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma_mdma_channel0_sw_0.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  hmdma_mdma_channel0_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  hmdma_mdma_channel0_sw_0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  hmdma_mdma_channel0_sw_0.Init.SourceBlockAddressOffset = 0;
  hmdma_mdma_channel0_sw_0.Init.DestBlockAddressOffset = 0;
  if (HAL_MDMA_Init(&hmdma_mdma_channel0_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_WORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_HALFWORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &sai_buffer_rx[1];
  nodeConfig.DstAddress = (uint32_t) &audio_buffer_rx_ch1_r[0];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel0_sw_1, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel0_sw_1 */
  HAL_MDMA_RegisterCallback(&hmdma_mdma_channel0_sw_0, HAL_MDMA_XFER_BLOCKCPLT_CB_ID, MDMA_RxXferBlockCpltCallback);
  HAL_MDMA_RegisterCallback(&hmdma_mdma_channel0_sw_0, HAL_MDMA_XFER_ERROR_CB_ID, MDMA_RxErrorCallback);
  /* USER CODE END mdma_channel0_sw_1 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel0_sw_0, &node_mdma_channel0_sw_1, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_WORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_HALFWORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &sai_buffer_rx[SAI_BUFFER_LENGTH_HALF-1];
  nodeConfig.DstAddress = (uint32_t) &audio_buffer_rx_ch1_l[AUDIO_BUFFER_LENGTH_HALF-1];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel0_sw_2, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel0_sw_2 */

  /* USER CODE END mdma_channel0_sw_2 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel0_sw_0, &node_mdma_channel0_sw_2, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_WORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_HALFWORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &sai_buffer_rx[SAI_BUFFER_LENGTH_HALF];
  nodeConfig.DstAddress = (uint32_t) &audio_buffer_rx_ch1_r[AUDIO_BUFFER_LENGTH_HALF-1];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel0_sw_3, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel0_sw_3 */

  /* USER CODE END mdma_channel0_sw_3 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel0_sw_0, &node_mdma_channel0_sw_3, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_WORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_HALFWORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &sai_buffer_rx[0];
  nodeConfig.DstAddress = (uint32_t) &audio_buffer_rx_ch1_l[0];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel0_sw_4, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel0_sw_4 */

  /* USER CODE END mdma_channel0_sw_4 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel0_sw_0, &node_mdma_channel0_sw_4, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Make the linked list circular by connecting the last node to the first */
  if (HAL_MDMA_LinkedList_EnableCircularMode(&hmdma_mdma_channel0_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure MDMA channel MDMA_Channel2 */
  /* Configure MDMA request hmdma_mdma_channel2_sw_0 on MDMA_Channel2 */
  hmdma_mdma_channel2_sw_0.Instance = MDMA_Channel2;
  hmdma_mdma_channel2_sw_0.Init.Request = MDMA_REQUEST_SW;
  hmdma_mdma_channel2_sw_0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  hmdma_mdma_channel2_sw_0.Init.Priority = MDMA_PRIORITY_MEDIUM;
  hmdma_mdma_channel2_sw_0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdma_mdma_channel2_sw_0.Init.SourceInc = MDMA_SRC_INC_HALFWORD;
  hmdma_mdma_channel2_sw_0.Init.DestinationInc = MDMA_DEST_INC_WORD;
  hmdma_mdma_channel2_sw_0.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  hmdma_mdma_channel2_sw_0.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  hmdma_mdma_channel2_sw_0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdma_mdma_channel2_sw_0.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  hmdma_mdma_channel2_sw_0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  hmdma_mdma_channel2_sw_0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  hmdma_mdma_channel2_sw_0.Init.SourceBlockAddressOffset = 0;
  hmdma_mdma_channel2_sw_0.Init.DestBlockAddressOffset = 0;
  if (HAL_MDMA_Init(&hmdma_mdma_channel2_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_MEDIUM;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_HALFWORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_WORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &audio_buffer_tx_ch1_r[0];
  nodeConfig.DstAddress = (uint32_t) &sai_buffer_tx[1];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel2_sw_1, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel2_sw_1 */
  HAL_MDMA_RegisterCallback(&hmdma_mdma_channel2_sw_0, HAL_MDMA_XFER_BLOCKCPLT_CB_ID, MDMA_TxXferBlockCpltCallback);
  HAL_MDMA_RegisterCallback(&hmdma_mdma_channel2_sw_0, HAL_MDMA_XFER_ERROR_CB_ID, MDMA_TxErrorCallback);
  /* USER CODE END mdma_channel2_sw_1 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel2_sw_0, &node_mdma_channel2_sw_1, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_MEDIUM;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_HALFWORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_WORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &audio_buffer_tx_ch1_l[AUDIO_BUFFER_LENGTH_HALF-1];
  nodeConfig.DstAddress = (uint32_t) &sai_buffer_tx[SAI_BUFFER_LENGTH_HALF-1];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel2_sw_2, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel2_sw_2 */

  /* USER CODE END mdma_channel2_sw_2 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel2_sw_0, &node_mdma_channel2_sw_2, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_MEDIUM;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_HALFWORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_WORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &audio_buffer_tx_ch1_r[AUDIO_BUFFER_LENGTH_HALF-1];
  nodeConfig.DstAddress = (uint32_t) &sai_buffer_tx[SAI_BUFFER_LENGTH_HALF];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel2_sw_3, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel2_sw_3 */

  /* USER CODE END mdma_channel2_sw_3 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel2_sw_0, &node_mdma_channel2_sw_3, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize MDMA link node according to specified parameters */
  nodeConfig.Init.Request = MDMA_REQUEST_SW;
  nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  nodeConfig.Init.Priority = MDMA_PRIORITY_MEDIUM;
  nodeConfig.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  nodeConfig.Init.SourceInc = MDMA_SRC_INC_HALFWORD;
  nodeConfig.Init.DestinationInc = MDMA_DEST_INC_WORD;
  nodeConfig.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
  nodeConfig.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
  nodeConfig.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  nodeConfig.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
  nodeConfig.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  nodeConfig.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  nodeConfig.Init.SourceBlockAddressOffset = 0;
  nodeConfig.Init.DestBlockAddressOffset = 0;
  nodeConfig.PostRequestMaskAddress = 0;
  nodeConfig.PostRequestMaskData = 0;
  nodeConfig.SrcAddress = (uint32_t) &audio_buffer_tx_ch1_l[0];
  nodeConfig.DstAddress = (uint32_t) &sai_buffer_tx[0];
  nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
  nodeConfig.BlockCount = 1;
  if (HAL_MDMA_LinkedList_CreateNode(&node_mdma_channel2_sw_4, &nodeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN mdma_channel2_sw_4 */

  /* USER CODE END mdma_channel2_sw_4 */

  /* Connect a node to the linked list */
  if (HAL_MDMA_LinkedList_AddNode(&hmdma_mdma_channel2_sw_0, &node_mdma_channel2_sw_4, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /* Make the linked list circular by connecting the last node to the first */
  if (HAL_MDMA_LinkedList_EnableCircularMode(&hmdma_mdma_channel2_sw_0) != HAL_OK)
  {
    Error_Handler();
  }

  /* MDMA interrupt initialization */
  /* MDMA_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MDMA_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(MDMA_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC1 PC4 PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : RESET_Pin */
  GPIO_InitStruct.Pin = RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PG11 PG13 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//TODO Adress/Size aligment error, Read error
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
	CLEAR_BIT(sai_status, SAI_STATUS_RX_HALF_PENDING);
	SET_BIT(sai_status, SAI_STATUS_RX_HALF_CPLT);
	CLEAR_BIT(sai_status, SAI_STATUS_RX_FULL_CPLT);
	SET_BIT(sai_status, SAI_STATUS_RX_FULL_PENDING);

	CLEAR_BIT(audio_rx_status, AUDIO_STATUS_HALF_PART);
	CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_HALF_PART);
	CLEAR_BIT(audio_tx_status, AUDIO_STATUS_HALF_PART);


}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
	CLEAR_BIT(sai_status, SAI_STATUS_RX_FULL_PENDING);
	SET_BIT(sai_status, SAI_STATUS_RX_FULL_CPLT);
	CLEAR_BIT(sai_status, SAI_STATUS_RX_HALF_CPLT);
	SET_BIT(sai_status, SAI_STATUS_RX_HALF_PENDING);

	CLEAR_BIT(audio_rx_status, AUDIO_STATUS_FULL);
	CLEAR_BIT(audio_dsp_status, AUDIO_STATUS_FULL);
	CLEAR_BIT(audio_tx_status, AUDIO_STATUS_FULL);


}

//void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
////	if(READ_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_CPLT) == false
////			&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_CPLT) == false){
////		SET_BIT(memoryTransferError, SAI_TX_UNDERRUN_ERROR);
////		Error_Handler();
////	}
//	//CLEAR_BIT(audio_tx_status, AUDIO_STATUS_FULL);
//
//}

//void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai) {
////	if(READ_BIT(audio_tx_status, AUDIO_STATUS_L_CPLT) == false
////			&& READ_BIT(audio_tx_status, AUDIO_STATUS_R_CPLT) == false){
////		SET_BIT(memoryTransferError, SAI_TX_UNDERRUN_ERROR);
////		Error_Handler();
////	}
//	//CLEAR_BIT(audio_tx_status, AUDIO_STATUS_HALF_PART);
//}

void MDMA_RxXferBlockCpltCallback(MDMA_HandleTypeDef *_hdma)
{
	transfercounter++;
	uint8_t nextNodeIndex = 0;
	MDMA_LinkNodeTypeDef* currNode = _hdma->FirstLinkedListNodeAddress;
	for(; nextNodeIndex < _hdma->LinkedListNodeCounter && currNode != (MDMA_LinkNodeTypeDef*)_hdma->Instance->CLAR; nextNodeIndex++, currNode = (MDMA_LinkNodeTypeDef*)currNode->CLAR)
	{
	}
	switch(nextNodeIndex){
	case 0:
		if(READ_BIT(audio_rx_status, AUDIO_STATUS_R_PENDING)){
			SET_BIT(audio_rx_status, AUDIO_STATUS_R_CPLT);
			CLEAR_BIT(audio_rx_status, AUDIO_STATUS_R_PENDING);
		}else{
			Error_Handler();
		}
		break;
	case 1:
		if(READ_BIT(audio_rx_status, AUDIO_STATUS_L_HALF_PENDING)){
			SET_BIT(audio_rx_status, AUDIO_STATUS_L_HALF_CPLT);
			CLEAR_BIT(audio_rx_status, AUDIO_STATUS_L_HALF_PENDING);
		}else{
			Error_Handler();
		}
		break;
	case 2:
		if(READ_BIT(audio_rx_status, AUDIO_STATUS_R_HALF_PENDING)){
			SET_BIT(audio_rx_status, AUDIO_STATUS_R_HALF_CPLT);
			CLEAR_BIT(audio_rx_status, AUDIO_STATUS_R_HALF_PENDING);
		}else{
			Error_Handler();
		}
		break;
	case 3:
		if(READ_BIT(audio_rx_status, AUDIO_STATUS_L_PENDING)){
			SET_BIT(audio_rx_status, AUDIO_STATUS_L_CPLT);
			CLEAR_BIT(audio_rx_status, AUDIO_STATUS_L_PENDING);
		}else{
			Error_Handler();
		}
		break;
	default:
		Error_Handler();
		break;
	}
}

void MDMA_TxXferBlockCpltCallback(MDMA_HandleTypeDef *_hdma)
{
	transfercounter++;
	uint8_t nextNodeIndex = 0;
	MDMA_LinkNodeTypeDef* currNode = _hdma->FirstLinkedListNodeAddress;
	for(; nextNodeIndex < _hdma->LinkedListNodeCounter && currNode != (MDMA_LinkNodeTypeDef*)_hdma->Instance->CLAR; nextNodeIndex++, currNode = (MDMA_LinkNodeTypeDef*)currNode->CLAR)
	{
	}
	switch(nextNodeIndex){
	case 0:
		if(READ_BIT(audio_tx_status, AUDIO_STATUS_R_PENDING)){
			SET_BIT(audio_tx_status, AUDIO_STATUS_R_CPLT);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_R_PENDING);
		}else{
			Error_Handler();
		}
		break;
	case 1:
		if(READ_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_PENDING)){
			SET_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_CPLT);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_L_HALF_PENDING);
		}else{
			Error_Handler();
		}
		break;
	case 2:
		if(READ_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_PENDING)){
			SET_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_CPLT);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_R_HALF_PENDING);
		}else{
			Error_Handler();
		}
		break;
	case 3:
		if(READ_BIT(audio_tx_status, AUDIO_STATUS_L_PENDING)){
			SET_BIT(audio_tx_status, AUDIO_STATUS_L_CPLT);
			CLEAR_BIT(audio_tx_status, AUDIO_STATUS_L_PENDING);
		}else{
			Error_Handler();
		}
		break;
	default:
		Error_Handler();
		break;
	}
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
