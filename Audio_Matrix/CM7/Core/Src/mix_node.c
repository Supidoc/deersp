/*
 * mix_node.c
 *
 *  Created on: May 25, 2025
 *      Author: dgrob
 */

#include "mix_node.h"

BUFMGR_status_t MIX_createNode(MIX_nodeConfig_t* config, NODE_node_t **generatedNode)
{
	if (config == NULL)
	{
		return BUFMGR_ERROR;
	}

	NODE_node_t *node = NULL;
	BUFMGR_status_t status = NDMGR_requestNodeSlot(&node);

	if (status != BUFMGR_OK)
	{
		return status;
	}

	node->nodeId = config->nodeId;
	node->nodeType = NODE_MIX_NODE;
	node->maxRefCount = 0;
	node->isProcessed = false;
	node->pOutputBuffer = NULL;
	node->inputNodeCount = 2;
	for (size_t i = 0; i < NODE_MAX_INPUT_NODES; i++)
	{
		node->pInputNodes[i] = NULL;
	}


	return BUFMGR_OK;
}

BUFMGR_status_t MIX_processNode(NODE_node_t *node)
{
	// Check if the node is valid
	if (node == NULL || node->data.mix == NULL)
	{
		return BUFMGR_ERROR;
	}

	float32_t *pOutputBuffer = node->pOutputBuffer->buffer;
	float32_t *inputBuffers[NODE_MAX_INPUT_NODES];
	for(size_t i = 0; i < node->inputNodeCount; i++)
	{
		if (node->pInputNodes[i] == NULL || node->pInputNodes[i]->pOutputBuffer == NULL)
		{
			return BUFMGR_ERROR;
		}
		inputBuffers[i] = node->pInputNodes[i]->pOutputBuffer->buffer;
	}


	// Mix the input buffers
	arm_add_f32(inputBuffers[0], inputBuffers[1], pOutputBuffer, AUDIO_BUFFER_LENGTH_HALF);

	return BUFMGR_OK;
}
