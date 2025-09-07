/*
 * biquad_node.c
 *
 *  Created on: May 23, 2025
 *      Author: dgrob
 */

#include "biquad_node.h"

BUFMGR_status_t BQD_createNode(BQD_nodeConfig_t* config, NODE_node_t **generatedNode)
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
	node->nodeType = NODE_BIQUAD_NODE;
	node->maxRefCount = 0;
	node->isProcessed = false;
	node->pOutputBuffer = NULL;
	node->inputNodeCount = 1;
	for(size_t i = 0; i < NODE_MAX_INPUT_NODES; i++)
	{
		node->pInputNodes[i] = NULL;
	}
	arm_biquad_cascade_df1_init_f32(&node->data.biquad->instance,
			config->stageCount, config->coeffs, node->data.biquad->state);

	*generatedNode = node;
	return BUFMGR_OK;
}

BUFMGR_status_t BQD_processNode(NODE_node_t *node)
{
	// Check if the node is valid
	if (node == NULL || node->data.biquad == NULL)
	{
		return BUFMGR_ERROR;
	}

	float32_t *pInputBuffer =node->pInputNodes[0]->pOutputBuffer->buffer;

	float32_t *pOutputBuffer = node->pOutputBuffer->buffer;

	arm_biquad_cascade_df1_f32(&node->data.biquad->instance, pInputBuffer,
			pOutputBuffer, AUDIO_BUFFER_LENGTH_HALF);

	node->maxRefCount--;

	return BUFMGR_OK;
}
