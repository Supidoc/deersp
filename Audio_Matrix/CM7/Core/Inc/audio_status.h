/*
 * audio_status.h
 *
 *  Created on: May 19, 2025
 *      Author: dgrob
 */

#ifndef INC_AUDIO_STATUS_H_
#define INC_AUDIO_STATUS_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_buffer.h"
#include "main.h"

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

extern volatile uint8_t ASTAT_rxStatus[AUDIO_STEREO_CHANNEL_COUNT];
extern volatile uint8_t ASTAT_dspStatus[AUDIO_STEREO_CHANNEL_COUNT];
extern volatile uint8_t ASTAT_txStatus[AUDIO_STEREO_CHANNEL_COUNT];
extern volatile uint8_t ASTAT_saiStatus;

extern volatile uint8_t memoryTransferError;

extern volatile bool ASTAT_isRxTransferComplete;
extern volatile bool ASTAT_isTxTransferComplete;

extern bool ASTAT_rxXferStartUpCplt;
extern bool ASTAT_txXferStartupCplt;

void ASTAT_init(MDMA_HandleTypeDef *hmdma_rx, MDMA_HandleTypeDef *hmdma_tx);

void ASTAT_reset(void);

void ASTAT_rxXferRequestHandler(void);
void ASTAT_DspRequestHandler(void);
void ASTAT_TxXferRequestHandler(void);

void ASTAT_RxXferBlockCpltHandler(void);
void ASTAT_TxXferBlockCpltHandler(void);

#endif /* INC_AUDIO_STATUS_H_ */
