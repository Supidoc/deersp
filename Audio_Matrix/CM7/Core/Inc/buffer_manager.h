/*
 * buffer_manager.h
 *
 *  Created on: May 24, 2025
 *      Author: dgrob
 */

#ifndef INC_BUFFER_MANAGER_H_
#define INC_BUFFER_MANAGER_H_
#include "audio_buffer.h"
#include "stdbool.h"


#define BUFMGR_MAX_BUFFER_COUNT 100



typedef struct _BUFMGR_nodeBuffer_t
{
	float32_t buffer[AUDIO_BUFFER_LENGTH_HALF];
	bool isUsed;
} BUFMGR_nodeBuffer_t;


typedef enum _BUFMGR_status_t
{
	BUFMGR_OK, BUFMGR_ERROR,
} BUFMGR_status_t;

BUFMGR_status_t BUFMGR_requestBuffer(BUFMGR_nodeBuffer_t **buffer);
BUFMGR_status_t BUFMGR_freeBuffer(BUFMGR_nodeBuffer_t *buffer);



#endif /* INC_BUFFER_MANAGER_H_ */
