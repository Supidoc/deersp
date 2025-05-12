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
#include <stdbool.h>

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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;

MDMA_HandleTypeDef hmdma_mdma_channel0_sw_0;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_1;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_2;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_3;
MDMA_LinkNodeTypeDef node_mdma_channel0_sw_4;
MDMA_HandleTypeDef hmdma_mdma_channel2_sw_0;
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_1;
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_2;
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_3;
MDMA_LinkNodeTypeDef node_mdma_channel2_sw_4;
/* USER CODE BEGIN PV */
TCA9548A_HandleTypeDef utca9548a1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_DMA_Init(void);
static void MX_MDMA_Init(void);
/* USER CODE BEGIN PFP */
static void MAIN_TCA9548A_Init(void);
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

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
	/*HW semaphore Clock enable*/
	__HAL_RCC_HSEM_CLK_ENABLE();
	/* Activate HSEM notification for Cortex-M4*/
	HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
	/*
	 Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
	 perform system initialization (system clock config, external memory configuration.. )
	 */
	HAL_PWREx_ClearPendingEvent();
	HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE,
	PWR_D2_DOMAIN);
	/* Clear HSEM flag */
	__HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_MDMA_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */
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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART3_UART_Init(void)
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

}

/* USER CODE BEGIN 4 */
/**
 * @brief TCA9548A Initialization Function
 * @param None
 * @retval None
 */
void MAIN_TCA9548A_Init(void) {

//	utca9548a1.hi2c = &hi2c2;
//	utca9548a1.rst_port = NULL;
//	utca9548a1.rst_pin = 0u;
//	utca9548a1.addr_offset = 0u;
	uint8_t nextNodeIndex = 0;
	MDMA_LinkNodeTypeDef* currNode = _hdma->FirstLinkedListNodeAddress;
	for(; nextNodeIndex < _hdma->LinkedListNodeCounter && currNode != (MDMA_LinkNodeTypeDef*)_hdma->Instance->CLAR; nextNodeIndex++, currNode = (MDMA_LinkNodeTypeDef*)currNode->CLAR)
	{
	}
	switch(nextNodeIndex){
	case 0:
		audio_rx_ch1_r_cplt = true;
		audio_rx_ch1_cplt = true;
		break;
	case 1:

		audio_rx_ch1_l_half_cplt = true;
		HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);
		break;
	case 2:
		audio_rx_ch1_r_half_cplt = true;
		audio_rx_ch1_half_cplt = true;
		break;
	case 3:
		audio_rx_ch1_l_cplt = true;
		HAL_MDMA_GenerateSWRequest(&hmdma_mdma_channel0_sw_0);
		break;
	}
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
