/*
 * audio_buffer.c
 *
 *  Created on: May 19, 2025
 *      Author: dgrob
 */
#include "audio_buffer.h"


__attribute__ ((aligned (8)))
static volatile q15_t ABUF_saiBufferRx[SAI_BUFFER_LENGTH] = {0};
__attribute__ ((aligned (8)))
static volatile q15_t ABUF_saiBufferTx[SAI_BUFFER_LENGTH] = {0};

ABUF_stereoBufferPtr_t ABUF_saiBufferPtrs[AUDIO_STEREO_CHANNEL_COUNT];

__attribute__((section(".audiobuffer"), aligned(8), used))
static ABUF_stereoBuffer_t ABUF_audioBuffers[AUDIO_STEREO_CHANNEL_COUNT];

ABUF_stereoBufferPtr_t ABUF_audioBufferPtrs[AUDIO_STEREO_CHANNEL_COUNT];

void ABUF_init(void){
	for(size_t i = 0;i<AUDIO_STEREO_CHANNEL_COUNT;i++){
		for(size_t j = 0; j< AUDIO_BUFFER_LENGTH; j++){
			ABUF_audioBuffers[i].leftRx[j] = 0;
			ABUF_audioBuffers[i].rightRx[j] = 0;
			ABUF_audioBuffers[i].leftTx[j] = 0;
			ABUF_audioBuffers[i].rightRx[j] = 0;
		}
		ABUF_saiBufferPtrs[i].leftRxFirstHalf = &ABUF_saiBufferRx[i*2];
		ABUF_saiBufferPtrs[i].rightRxFirstHalf = &ABUF_saiBufferRx[i*2+1];
		ABUF_saiBufferPtrs[i].leftRxSecHalf = &ABUF_saiBufferRx[SAI_BUFFER_LENGTH_HALF + i*2];
		ABUF_saiBufferPtrs[i].rightRxSecHalf = &ABUF_saiBufferRx[SAI_BUFFER_LENGTH_HALF + i*2 + 1];
		ABUF_saiBufferPtrs[i].leftTxFirstHalf = &ABUF_saiBufferTx[i*2];
		ABUF_saiBufferPtrs[i].rightTxFirstHalf = &ABUF_saiBufferTx[i*2+1];
		ABUF_saiBufferPtrs[i].leftTxSecHalf = &ABUF_saiBufferTx[SAI_BUFFER_LENGTH_HALF + i*2];
		ABUF_saiBufferPtrs[i].rightTxSecHalf = &ABUF_saiBufferTx[SAI_BUFFER_LENGTH_HALF + i*2 + 1];

		ABUF_audioBufferPtrs[i].leftRxFirstHalf = &ABUF_audioBuffers[i].leftRx[0];
		ABUF_audioBufferPtrs[i].rightRxFirstHalf = &ABUF_audioBuffers[i].rightRx[0];
		ABUF_audioBufferPtrs[i].leftRxSecHalf = &ABUF_audioBuffers[i].leftRx[AUDIO_BUFFER_LENGTH];
		ABUF_audioBufferPtrs[i].rightRxSecHalf = &ABUF_audioBuffers[i].rightRx[AUDIO_BUFFER_LENGTH];
		ABUF_audioBufferPtrs[i].leftTxFirstHalf = &ABUF_audioBuffers[i].leftTx[0];
		ABUF_audioBufferPtrs[i].rightTxFirstHalf = &ABUF_audioBuffers[i].rightTx[0];
		ABUF_audioBufferPtrs[i].leftTxSecHalf = &ABUF_audioBuffers[i].leftTx[AUDIO_BUFFER_LENGTH];
		ABUF_audioBufferPtrs[i].rightTxSecHalf = &ABUF_audioBuffers[i].rightTx[AUDIO_BUFFER_LENGTH];


	}
}
