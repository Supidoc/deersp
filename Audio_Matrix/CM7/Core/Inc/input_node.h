/*
 * input_node.h
 *
 *  Created on: May 28, 2025
 *      Author: dgrob
 */

#ifndef INC_INPUT_NODE_H_
#define INC_INPUT_NODE_H_

#include "arm_math.h"
#include "main.h"
#include "audio_buffer.h"
#include "node_manager.h"

typedef struct _INPUT_nodeData
{
	volatile q15_t* inputBufferFirstHalf;
	volatile q15_t* inputBufferSecHalf;
} INPUT_nodeData_t;


typedef struct _INPUT_nodeConfig
{
	uint16_t nodeId;
	uint8_t channelIndex;
	ABUF_channelSide_t channelSide;
} INPUT_nodeConfig_t;

BUFMGR_status_t INPUT_createNode(INPUT_nodeConfig_t* config, NODE_node_t **generatedNode);
BUFMGR_status_t INPUT_processNode(NODE_node_t *node);

#endif /* INC_INPUT_NODE_H_ */
