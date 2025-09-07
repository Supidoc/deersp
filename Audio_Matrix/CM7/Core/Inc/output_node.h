/*
 * output_node.h
 *
 *  Created on: Aug 9, 2025
 *      Author: dgrob
 */

#ifndef INC_OUTPUT_NODE_H_
#define INC_OUTPUT_NODE_H_

#include "arm_math.h"
#include "main.h"
#include "audio_buffer.h"
#include "node_manager.h"


typedef struct _OUTPUT_nodeData
{
	volatile q15_t* outputBufferFirstHalf;
	volatile q15_t* outputBufferSecHalf;
} OUTPUT_nodeData_t;



typedef struct _OUTPUT_nodeConfig
{
	uint16_t nodeId;
	uint8_t channelIndex;
	ABUF_channelSide_t channelSide;
} OUPUT_nodeConfig_t;

BUFMGR_status_t OUTPUT_createNode(OUPUT_nodeConfig_t* config, NODE_node_t **generatedNode);
BUFMGR_status_t OUTPUT_processNode(NODE_node_t *node);

#endif /* INC_OUTPUT_NODE_H_ */
