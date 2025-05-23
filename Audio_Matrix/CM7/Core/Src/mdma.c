/*
 * mdma.c
 *
 *  Created on: May 19, 2025
 *      Author: dgrob
 */

#include "mdma.h"
#include "audio_buffer.h"
__attribute__ ((aligned (8)))
static MDMA_HandleTypeDef MDMA_ch0Sw0;
__attribute__ ((aligned (8)))
MDMA_LinkNodeTypeDef MDMA_ch0Nodes[AUDIO_CHANNEL_COUNT];
__attribute__ ((aligned (8)))
static MDMA_HandleTypeDef MDMA_ch1Sw0;
__attribute__ ((aligned (8)))
MDMA_LinkNodeTypeDef MDMA_ch1Nodes[AUDIO_CHANNEL_COUNT];

MDMA_HandleTypeDef *MDMA_hmdmaRxInstance;
MDMA_HandleTypeDef *MDMA_hmdmaTxInstance;

void MDMA_init(void)
{
	__HAL_RCC_MDMA_CLK_ENABLE();
	/* Local variables */
	MDMA_LinkNodeConfTypeDef nodeConfig;

	/* Configure MDMA channel MDMA_Channel0 */
	MDMA_ch0Sw0.Instance = MDMA_Channel0;
	MDMA_ch0Sw0.Init.Request = MDMA_REQUEST_SW;
	MDMA_ch0Sw0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
	MDMA_ch0Sw0.Init.Priority = MDMA_PRIORITY_HIGH;
	MDMA_ch0Sw0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
	MDMA_ch0Sw0.Init.SourceInc = MDMA_SRC_INC_WORD;
	MDMA_ch0Sw0.Init.DestinationInc = MDMA_DEST_INC_HALFWORD;
	MDMA_ch0Sw0.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
	MDMA_ch0Sw0.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
	MDMA_ch0Sw0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
	MDMA_ch0Sw0.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
	MDMA_ch0Sw0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
	MDMA_ch0Sw0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
	MDMA_ch0Sw0.Init.SourceBlockAddressOffset = 0;
	MDMA_ch0Sw0.Init.DestBlockAddressOffset = 0;
	if (HAL_MDMA_Init(&MDMA_ch0Sw0) != HAL_OK)
	{
		Error_Handler();
	}

	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT;)
	{
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
		nodeConfig.SrcAddress =
				(uint32_t) ABUF_saiBufferPtrs[i].leftRxFirstHalf;
		nodeConfig.DstAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].leftRxFirstHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(&MDMA_ch0Nodes[i * 2], &nodeConfig)
				!= HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch0Sw0, &MDMA_ch0Nodes[i * 2], 0)
				!= HAL_OK)
		{
			Error_Handler();
		}
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
		nodeConfig.SrcAddress =
				(uint32_t) ABUF_saiBufferPtrs[i].rightRxFirstHalf;
		nodeConfig.DstAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].rightRxFirstHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(&MDMA_ch0Nodes[i * 2 + 1],
				&nodeConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch0Sw0, &MDMA_ch0Nodes[i * 2 + 1],
				0) != HAL_OK)
		{
			Error_Handler();
		}
	}
	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT;)
	{
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
		nodeConfig.SrcAddress = (uint32_t) ABUF_saiBufferPtrs[i].leftRxSecHalf;
		nodeConfig.DstAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].leftRxSecHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(
				&MDMA_ch0Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2],
				&nodeConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch0Sw0,
				&MDMA_ch0Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2], 0)
				!= HAL_OK)
		{
			Error_Handler();
		}
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
		nodeConfig.SrcAddress = (uint32_t) ABUF_saiBufferPtrs[i].rightRxSecHalf;
		nodeConfig.DstAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].rightRxSecHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(
				&MDMA_ch0Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2 + 1],
				&nodeConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch0Sw0,
				&MDMA_ch0Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2 + 1], 0)
				!= HAL_OK)
		{
			Error_Handler();
		}
	}

	if (HAL_MDMA_LinkedList_EnableCircularMode(&MDMA_ch0Sw0) != HAL_OK)
	{
		Error_Handler();
	}

	/* Configure MDMA channel MDMA_Channel1 */
	MDMA_ch1Sw0.Instance = MDMA_Channel1;
	MDMA_ch1Sw0.Init.Request = MDMA_REQUEST_SW;
	MDMA_ch1Sw0.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
	MDMA_ch1Sw0.Init.Priority = MDMA_PRIORITY_HIGH;
	MDMA_ch1Sw0.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
	MDMA_ch1Sw0.Init.SourceInc = MDMA_SRC_INC_HALFWORD;
	MDMA_ch1Sw0.Init.DestinationInc = MDMA_DEST_INC_WORD;
	MDMA_ch1Sw0.Init.SourceDataSize = MDMA_SRC_DATASIZE_HALFWORD;
	MDMA_ch1Sw0.Init.DestDataSize = MDMA_DEST_DATASIZE_HALFWORD;
	MDMA_ch1Sw0.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
	MDMA_ch1Sw0.Init.BufferTransferLength = AUDIO_SAMPLE_SIZE;
	MDMA_ch1Sw0.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
	MDMA_ch1Sw0.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
	MDMA_ch1Sw0.Init.SourceBlockAddressOffset = 0;
	MDMA_ch1Sw0.Init.DestBlockAddressOffset = 0;
	if (HAL_MDMA_Init(&MDMA_ch1Sw0) != HAL_OK)
	{
		Error_Handler();
	}

	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT;)
	{
		nodeConfig.Init.Request = MDMA_REQUEST_SW;
		nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
		nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
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
		nodeConfig.SrcAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].leftTxFirstHalf;
		nodeConfig.DstAddress =
				(uint32_t) ABUF_saiBufferPtrs[i].leftTxFirstHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(&MDMA_ch1Nodes[i * 2], &nodeConfig)
				!= HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch1Sw0, &MDMA_ch1Nodes[i * 2], 0)
				!= HAL_OK)
		{
			Error_Handler();
		}
		nodeConfig.Init.Request = MDMA_REQUEST_SW;
		nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
		nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
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
		nodeConfig.SrcAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].rightTxFirstHalf;
		nodeConfig.DstAddress =
				(uint32_t) ABUF_saiBufferPtrs[i].rightTxFirstHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(&MDMA_ch1Nodes[i * 2 + 1],
				&nodeConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch1Sw0, &MDMA_ch1Nodes[i * 2 + 1],
				0) != HAL_OK)
		{
			Error_Handler();
		}
	}
	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT;)
	{
		nodeConfig.Init.Request = MDMA_REQUEST_SW;
		nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
		nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
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
		nodeConfig.SrcAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].leftTxSecHalf;
		nodeConfig.DstAddress = (uint32_t) ABUF_saiBufferPtrs[i].leftTxSecHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(
				&MDMA_ch1Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2],
				&nodeConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch1Sw0,
				&MDMA_ch1Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2], 0)
				!= HAL_OK)
		{
			Error_Handler();
		}
		nodeConfig.Init.Request = MDMA_REQUEST_SW;
		nodeConfig.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
		nodeConfig.Init.Priority = MDMA_PRIORITY_HIGH;
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
		nodeConfig.SrcAddress =
				(uint32_t) ABUF_audioBufferPtrs[i].rightTxSecHalf;
		nodeConfig.DstAddress = (uint32_t) ABUF_saiBufferPtrs[i].rightTxSecHalf;
		nodeConfig.BlockDataLength = AUDIO_BUFFER_SIZE_HALF;
		nodeConfig.BlockCount = 1;
		if (HAL_MDMA_LinkedList_CreateNode(
				&MDMA_ch1Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2 + 1],
				&nodeConfig) != HAL_OK)
		{
			Error_Handler();
		}
		if (HAL_MDMA_LinkedList_AddNode(&MDMA_ch1Sw0,
				&MDMA_ch1Nodes[AUDIO_STEREO_CHANNEL_COUNT * 2 + i * 2 + 1], 0)
				!= HAL_OK)
		{
			Error_Handler();
		}
	}

	if (HAL_MDMA_LinkedList_EnableCircularMode(&MDMA_ch1Sw0) != HAL_OK)
	{
		Error_Handler();
	}

	/* MDMA interrupt initialization */
	/* MDMA_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(MDMA_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(MDMA_IRQn);

	/* Set the MDMA instance for the RX and TX */
	MDMA_hmdmaRxInstance = &MDMA_ch0Sw0;
	MDMA_hmdmaTxInstance = &MDMA_ch1Sw0;
}

HAL_StatusTypeDef MDMA_registerRxCallbacks(
		void (*pXferCallback)(MDMA_HandleTypeDef *_hmdma),
		void (*pErrorCallback)(MDMA_HandleTypeDef *_hmdma))
{
	HAL_StatusTypeDef status;

	/* Register transfer callback */
	status = HAL_MDMA_RegisterCallback(&MDMA_ch0Sw0, HAL_MDMA_XFER_ALL_CB_ID,
			pXferCallback);
	if (status != HAL_OK)
	{
		return status;
	}
	/* Register the error callback */
	return HAL_MDMA_RegisterCallback(&MDMA_ch0Sw0, HAL_MDMA_XFER_ERROR_CB_ID,
			pErrorCallback);
}

HAL_StatusTypeDef MDMA_registerTxCallbacks(
		void (*pXferCallback)(MDMA_HandleTypeDef *_hmdma),
		void (*pErrorCallback)(MDMA_HandleTypeDef *_hmdma))
{
	HAL_StatusTypeDef status;

	/* Register transfer callback */
	status = HAL_MDMA_RegisterCallback(&MDMA_ch1Sw0, HAL_MDMA_XFER_ALL_CB_ID,
			pXferCallback);
	if (status != HAL_OK)
	{
		return status;
	}
	/* Register the error callback */
	return HAL_MDMA_RegisterCallback(&MDMA_ch1Sw0, HAL_MDMA_XFER_ERROR_CB_ID,
			pErrorCallback);
}

HAL_StatusTypeDef MDMA_startRxTransfer(void)
{
	/* Start the MDMA transfer */
	return HAL_MDMA_Start_IT(MDMA_hmdmaRxInstance,
			(uint32_t) ABUF_saiBufferPtrs[0].leftRxFirstHalf,
			(uint32_t) ABUF_audioBufferPtrs[0].leftRxFirstHalf,
			AUDIO_BUFFER_SIZE_HALF, 1);
}

HAL_StatusTypeDef MDMA_startTxTransfer(void)
{
	/* Start the MDMA transfer */
	return HAL_MDMA_Start_IT(MDMA_hmdmaTxInstance,
			(uint32_t) ABUF_audioBufferPtrs[0].leftTxFirstHalf,
			(uint32_t) ABUF_saiBufferPtrs[0].leftTxFirstHalf,
			AUDIO_BUFFER_SIZE_HALF, 1);
}

uint8_t MDMA_getCurrentRxChIndex(void)
{

	/* Get the current channel index */
	uint8_t currNodeIndex = 0;
	MDMA_LinkNodeTypeDef *currNode = MDMA_hmdmaRxInstance->FirstLinkedListNodeAddress;
	for (;
			currNodeIndex < MDMA_hmdmaRxInstance->LinkedListNodeCounter
					&& currNode
							!= (MDMA_LinkNodeTypeDef*) MDMA_hmdmaRxInstance->Instance->CLAR;
			currNodeIndex++, currNode = (MDMA_LinkNodeTypeDef*) currNode->CLAR)
	{}
	return currNodeIndex;
}

uint8_t MDMA_getCurrentTxChIndex(void)
{

	/* Get the current channel index */
	uint8_t currNodeIndex = 0;
	MDMA_LinkNodeTypeDef *currNode = MDMA_hmdmaTxInstance->FirstLinkedListNodeAddress;
	for (;
			currNodeIndex < MDMA_hmdmaTxInstance->LinkedListNodeCounter
					&& currNode
							!= (MDMA_LinkNodeTypeDef*) MDMA_hmdmaTxInstance->Instance->CLAR;
			currNodeIndex++, currNode = (MDMA_LinkNodeTypeDef*) currNode->CLAR)
	{}
	return currNodeIndex;
}
