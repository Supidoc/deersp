/*
 * audio_status.c
 *
 *  Created on: May 19, 2025
 *      Author: dgrob
 */

#include "audio_status.h"
#include "mdma.h"
#include "main.h"
#include "stdbool.h"

volatile uint8_t ASTAT_rxStatus[AUDIO_STEREO_CHANNEL_COUNT] =
{ 0 };
volatile uint8_t ASTAT_dspStatus[AUDIO_STEREO_CHANNEL_COUNT] =
{ 0 };
volatile uint8_t ASTAT_txStatus[AUDIO_STEREO_CHANNEL_COUNT] =
{ 0 };
volatile uint8_t ASTAT_saiStatus = 0;

volatile uint8_t memoryTransferError = 0;

static MDMA_HandleTypeDef *ASTAT_hmdmaRxInstance;
static MDMA_HandleTypeDef *ASTAT_hmdmaTxInstance;

volatile bool ASTAT_isRxTransferComplete = false;
volatile bool ASTAT_isTxTransferComplete = false;

bool ASTAT_rxXferStartUpCplt = false;
bool ASTAT_txXferStartupCplt = false;

void ASTAT_init(MDMA_HandleTypeDef *hmdma_rx, MDMA_HandleTypeDef *hmdma_tx)
{
	ASTAT_hmdmaRxInstance = hmdma_rx;
	ASTAT_hmdmaTxInstance = hmdma_tx;

	ASTAT_reset();
}

void ASTAT_reset(void)
{
	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{
		ASTAT_rxStatus[i] = 0;
		ASTAT_dspStatus[i] = 0;
		ASTAT_txStatus[i] = 0;
	}

	ASTAT_saiStatus = 0;

}

void ASTAT_rxXferRequestHandler(void)
{

	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{

		__disable_irq();

		if (READ_BIT(ASTAT_saiStatus,
				SAI_STATUS_RX_HALF_CPLT) && READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_L_HALF_PENDING) == false
				&& READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_L_HALF_CPLT) == false)
		{
			//TODO Check if previous TX was completed
			__enable_irq();
			SET_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_L_HALF_PENDING);

			if (ASTAT_rxXferStartUpCplt)
			{
				HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaRxInstance);
			}
			else
			{
				ASTAT_rxXferStartUpCplt = true;
				MDMA_startRxTransfer();
			}

		}
		else if (READ_BIT(ASTAT_rxStatus[i],
				AUDIO_STATUS_L_HALF_CPLT) && READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_HALF_PENDING) == false
				&& READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_HALF_PENDING);

			HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaRxInstance);
		}
		else if (READ_BIT(ASTAT_saiStatus,
				SAI_STATUS_RX_FULL_CPLT) && READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_L_PENDING) == false
				&& READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_L_CPLT) == false)
		{
			//TODO Check if previous TX was completed
			__enable_irq();

			SET_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_L_PENDING);

			HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaRxInstance);
		}
		else if (READ_BIT(ASTAT_rxStatus[i],
				AUDIO_STATUS_L_CPLT) && READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_PENDING) == false
				&& READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_PENDING);

			HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaRxInstance);
		}
		else
		{
			__enable_irq();
		}
	}
}

void ASTAT_DspRequestHandler(void)
{
	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{

		__disable_irq();
		if (READ_BIT(ASTAT_rxStatus[i],
				AUDIO_STATUS_L_HALF_CPLT) && READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_HALF_PENDING) == false
				&& READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_HALF_CPLT) == false)
		{
			__enable_irq();
			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_HALF_PENDING);
			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_HALF_CPLT);

		}
		else if (READ_BIT(ASTAT_rxStatus[i],
				AUDIO_STATUS_R_HALF_CPLT) && READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_HALF_PENDING) == false
				&& READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_HALF_PENDING);
			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_HALF_CPLT);

		}
		else if (READ_BIT(ASTAT_rxStatus[i],
				AUDIO_STATUS_L_CPLT) && READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_PENDING) == false
				&& READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_PENDING);
			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_L_CPLT);

		}
		else if (READ_BIT(ASTAT_rxStatus[i],
				AUDIO_STATUS_R_CPLT) && READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_PENDING) == false
				&& READ_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_PENDING);
			CLEAR_BIT(ASTAT_dspStatus[i], AUDIO_STATUS_R_CPLT);
		}
		else
		{
			__enable_irq();
		}
	}
}

void ASTAT_TxXferRequestHandler(void)
{
	for (size_t i = 0; i < AUDIO_STEREO_CHANNEL_COUNT; i++)
	{
		__disable_irq();
		if (READ_BIT(ASTAT_dspStatus[i],
				AUDIO_STATUS_L_HALF_CPLT) && READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_HALF_CPLT)
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_HALF_PENDING) == false
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_HALF_PENDING);
			CLEAR_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_HALF_CPLT);

			if (ASTAT_txXferStartupCplt)
			{
				HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaTxInstance);
			}
			else
			{
				ASTAT_txXferStartupCplt = true;
				MDMA_startTxTransfer();
			}
		}
		else if (READ_BIT(ASTAT_dspStatus[i],
				AUDIO_STATUS_R_HALF_CPLT) && READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_HALF_CPLT)
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_HALF_PENDING) == false
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_HALF_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_HALF_PENDING);
			CLEAR_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_HALF_CPLT);
			HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaTxInstance);
		}
		else if (READ_BIT(ASTAT_dspStatus[i],
				AUDIO_STATUS_L_CPLT) && READ_BIT(ASTAT_rxStatus[i], AUDIO_STATUS_R_CPLT)
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_PENDING) == false
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_PENDING);
			CLEAR_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_CPLT);
			HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaTxInstance);
		}
		else if (READ_BIT(ASTAT_dspStatus[i],
				AUDIO_STATUS_R_CPLT) && READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_L_CPLT)
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_PENDING) == false
				&& READ_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_CPLT) == false)
		{
			__enable_irq();

			SET_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_PENDING);
			CLEAR_BIT(ASTAT_txStatus[i], AUDIO_STATUS_R_CPLT);
			HAL_MDMA_GenerateSWRequest(ASTAT_hmdmaTxInstance);
		}
		else
		{
			__enable_irq();
		}
	}
}

void ASTAT_RxXferBlockCpltHandler(void)
{
	if (ASTAT_isRxTransferComplete)
	{
		ASTAT_isRxTransferComplete = false;

		uint8_t currNodeIndex = MDMA_getCurrentRxChIndex();

		bool isFirstHalf = false;
		uint8_t currChIndex;
		if ((currNodeIndex) / 2 < AUDIO_STEREO_CHANNEL_COUNT)
		{
			currChIndex = currNodeIndex / 2;
			isFirstHalf = true;
		}
		else
		{
			currChIndex = (currNodeIndex - AUDIO_CHANNEL_COUNT) / 2;
		}

		bool isLeft = (currNodeIndex % 2) == 0 ;

		if (isFirstHalf)
		{
			if (isLeft)
			{
				if (READ_BIT(ASTAT_rxStatus[currChIndex],
						AUDIO_STATUS_L_HALF_PENDING))
				{
					SET_BIT(ASTAT_rxStatus[currChIndex],
							AUDIO_STATUS_L_HALF_CPLT);
					CLEAR_BIT(ASTAT_rxStatus[currChIndex],
							AUDIO_STATUS_L_HALF_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}
			else
			{
				if (READ_BIT(ASTAT_rxStatus[currChIndex],
						AUDIO_STATUS_R_HALF_PENDING))
				{
					SET_BIT(ASTAT_rxStatus[currChIndex],
							AUDIO_STATUS_R_HALF_CPLT);
					CLEAR_BIT(ASTAT_rxStatus[currChIndex],
							AUDIO_STATUS_R_HALF_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}
		}
		else
		{
			if (isLeft)
			{
				if (READ_BIT(ASTAT_rxStatus[currChIndex],
						AUDIO_STATUS_L_PENDING))
				{
					SET_BIT(ASTAT_rxStatus[currChIndex], AUDIO_STATUS_L_CPLT);
					CLEAR_BIT(ASTAT_rxStatus[currChIndex],
							AUDIO_STATUS_L_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}
			else
			{
				if (READ_BIT(ASTAT_rxStatus[currChIndex],
						AUDIO_STATUS_R_PENDING))
				{
					SET_BIT(ASTAT_rxStatus[currChIndex], AUDIO_STATUS_R_CPLT);
					CLEAR_BIT(ASTAT_rxStatus[currChIndex],
							AUDIO_STATUS_R_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}
		}

	}
}

void ASTAT_TxXferBlockCpltHandler(void)
{
	if (ASTAT_isTxTransferComplete)
	{
		ASTAT_isTxTransferComplete = false;

		uint8_t currNodeIndex = MDMA_getCurrentTxChIndex();

		bool isFirstHalf = false;
		uint8_t currChIndex;
		if (currNodeIndex / 2 < AUDIO_STEREO_CHANNEL_COUNT)
		{
			currChIndex = currNodeIndex / 2 ;
			isFirstHalf = true;
		}
		else
		{
			currChIndex = (currNodeIndex - AUDIO_CHANNEL_COUNT) / 2;
		}

		bool isLeft = (currNodeIndex % 2) == 0;

		if (isFirstHalf)
		{
			if (isLeft)
			{
				if (READ_BIT(ASTAT_txStatus[currChIndex],
						AUDIO_STATUS_L_HALF_PENDING))
				{
					SET_BIT(ASTAT_txStatus[currChIndex],
							AUDIO_STATUS_L_HALF_CPLT);
					CLEAR_BIT(ASTAT_txStatus[currChIndex],
							AUDIO_STATUS_L_HALF_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}
			else
			{
				if (READ_BIT(ASTAT_txStatus[currChIndex],
						AUDIO_STATUS_R_HALF_PENDING))
				{
					SET_BIT(ASTAT_txStatus[currChIndex],
							AUDIO_STATUS_R_HALF_CPLT);
					CLEAR_BIT(ASTAT_txStatus[currChIndex],
							AUDIO_STATUS_R_HALF_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}
		}
		else
		{
			if (isLeft)
			{
				if (READ_BIT(ASTAT_txStatus[currChIndex],
						AUDIO_STATUS_L_PENDING))
				{
					SET_BIT(ASTAT_txStatus[currChIndex], AUDIO_STATUS_L_CPLT);
					CLEAR_BIT(ASTAT_txStatus[currChIndex],
							AUDIO_STATUS_L_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}
			else
			{
				if (READ_BIT(ASTAT_txStatus[currChIndex],
						AUDIO_STATUS_R_PENDING))
				{
					SET_BIT(ASTAT_txStatus[currChIndex], AUDIO_STATUS_R_CPLT);
					CLEAR_BIT(ASTAT_txStatus[currChIndex],
							AUDIO_STATUS_R_PENDING);
				}
				else
				{
					Error_Handler();
				}
			}

		}
	}
}
