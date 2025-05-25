/*
 * buffer_manager.h
 *
 *  Created on: May 24, 2025
 *      Author: dgrob
 */

#ifndef INC_BUFFER_MANAGER_H_
#define INC_BUFFER_MANAGER_H_
#include "audio_buffer.h"

#define BUFFER_MANAGER_MAX_BUFFER_COUNT 100

// Forward declaration of BQD_nodeData_t
typedef struct _BQD_nodeData BQD_nodeData_t;

typedef struct _BUFMGR_nodeBuffer_t {
uint8_t refCount;
float32_t buffer[AUDIO_BUFFER_LENGTH_HALF];
} BUFMGR_nodeBuffer_t;

typedef enum _BUFMGR_nodeType_t{
	BUFMGR_BIQUAD_NODE,
	BUFMGR_MIX_NODE,
} BUFMGR_nodeType_t;

typedef struct _BUFMGR_node_t {
	BUFMGR_nodeBuffer_t* outputBuffer;
	BUFMGR_nodeType_t nodeType;
	uint8_t refCount;
	union {
		BQD_nodeData_t* biquad;
	} data;
} BUFMGR_node_t;



typedef enum _BUFMGR_status_t{
	BUFMGR_OK,
	BUFMGR_ERROR,
} BUFMGR_status_t;


BUFMGR_status_t BUFMGR_requestBuffer(BUFMGR_node_t* node, BUFMGR_nodeType_t nodeType);
BUFMGR_status_t BUFMGR_reduceBufferRefCount(BUFMGR_node_t* node, BUFMGR_nodeType_t nodeType);

BUFMGR_status_t BUFMGR_createReference(BUFMGR_node_t* childNode, BUFMGR_node_t* parentNode);


#endif /* INC_BUFFER_MANAGER_H_ */
