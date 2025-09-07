/*
 * graph_manager.c
 *
 *  Created on: Aug 13, 2025
 *      Author: dgrob
 */

#include "graph_manager.h"
#include "audio_buffer.h"
#include "buffer_manager.h"

NODE_node_t *GRPMGR_inputNodes[INPUT_NODE_COUNT] = {NULL};
NODE_node_t *GRPMGR_outputNodes[OUTPUT_NODE_COUNT] = {NULL};


GRPMGR_status_t GRPMGR_GenerateExampleGraph(void)
{

	INPUT_nodeConfig_t inputConfig_1 =
	{
		.nodeId = 1,
		.channelIndex = 0,
		.channelSide = ABUF_CHANNEL_LEFT
	};
	NODE_node_t *node = NULL;
	INPUT_createNode(&inputConfig_1,&node);

	GRPMGR_inputNodes[0] = node;

	return GRPMGR_OK;
}

GRPMGR_status_t GRPMGR_processGraph(void){

	for(size_t i = 0; i<OUTPUT_NODE_COUNT; i++){
		NDMGR_processNode(GRPMGR_outputNodes[i]);
	}

	return GRPMGR_OK;
}

GRPMGR_status_t GRPMGR_linkNodes(uint16_t childNodeId,
		uint16_t parentNodeId, uint8_t inputNodeIndex)
{

	NODE_node_t *childNode = NULL;
	NODE_node_t *parentNode = NULL;

	for(size_t i = 0; i < NDMGR_MAX_NODE_COUNT; i++)
	{
		if(NDMGR_nodePool[i].nodeId == childNodeId)
		{
			childNode = &NDMGR_nodePool[i];
		}
		if(NDMGR_nodePool[i].nodeId == parentNodeId)
		{
			parentNode = &NDMGR_nodePool[i];
		}
		if(childNode != NULL && parentNode != NULL)
		{
			break;
		}
	}
	if(childNode == NULL || parentNode == NULL)
	{
		return GRPMGR_ERROR; // One of the nodes was not found
	}

	childNode->pInputNodes[inputNodeIndex] = parentNode;

	parentNode->maxRefCount++;


	return BUFMGR_OK;
}
