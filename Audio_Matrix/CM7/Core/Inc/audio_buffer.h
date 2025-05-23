/*
 * audio_buffer.h
 *
 *  Created on: May 19, 2025
 *      Author: dgrob
 */

#ifndef INC_AUDIO_BUFFER_H_
#define INC_AUDIO_BUFFER_H_

#include "arm_math.h"
#define AUDIO_STEREO_CHANNEL_COUNT 1
#define AUDIO_CHANNEL_COUNT AUDIO_STEREO_CHANNEL_COUNT*2
#define AUDIO_BUFFER_LENGTH 256
#define AUDIO_SAMPLE_SIZE sizeof(q15_t)
#define AUDIO_BUFFER_SIZE AUDIO_BUFFER_LENGTH*AUDIO_SAMPLE_SIZE
#define AUDIO_BUFFER_LENGTH_HALF AUDIO_BUFFER_LENGTH/2
#define AUDIO_BUFFER_SIZE_HALF AUDIO_BUFFER_SIZE/2

#define SAI_BUFFER_LENGTH AUDIO_BUFFER_LENGTH*AUDIO_CHANNEL_COUNT
#define SAI_BUFFER_SIZE SAI_BUFFER_LENGTH*AUDIO_SAMPLE_SIZE
#define SAI_BUFFER_LENGTH_HALF SAI_BUFFER_LENGTH/2


typedef struct _ABUF_stereoBuffer{

	__attribute__((aligned(8)))  volatile q15_t leftRx[AUDIO_BUFFER_LENGTH];
	__attribute__((aligned(8)))  volatile q15_t rightRx[AUDIO_BUFFER_LENGTH];
	__attribute__((aligned(8)))  volatile q15_t leftTx[AUDIO_BUFFER_LENGTH];
	__attribute__((aligned(8)))  volatile q15_t rightTx[AUDIO_BUFFER_LENGTH];

} ABUF_stereoBuffer_t;

typedef struct _ABUF_stereoBufferPtr{

	volatile q15_t* leftRxFirstHalf;
	volatile q15_t* rightRxFirstHalf;
	volatile q15_t* leftRxSecHalf;
	volatile q15_t* rightRxSecHalf;
	volatile q15_t* leftTxFirstHalf;
	volatile q15_t* rightTxFirstHalf;
	volatile q15_t* leftTxSecHalf;
	volatile q15_t* rightTxSecHalf;

} ABUF_stereoBufferPtr_t;



extern ABUF_stereoBufferPtr_t ABUF_saiBufferPtrs[AUDIO_STEREO_CHANNEL_COUNT];

extern ABUF_stereoBufferPtr_t ABUF_audioBufferPtrs[AUDIO_STEREO_CHANNEL_COUNT];

void ABUF_init(void);

#endif /* INC_AUDIO_BUFFER_H_ */
