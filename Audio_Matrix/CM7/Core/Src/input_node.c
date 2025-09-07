/*
 * input_node.c
 *
 *  Created on: May 28, 2025
 *      Author: dgrob
 */

#include "input_node.h"
#include "buffer_manager.h"
#include "dsp.h"

BUFMGR_status_t INPUT_createNode(INPUT_nodeConfig_t* config, NODE_node_t **generatedNode)
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
	node->nodeType = NODE_INPUT_NODE;
	node->maxRefCount = 0;
	node->isProcessed = false;
	if(config->channelSide == ABUF_CHANNEL_LEFT)
	{
		node->data.input->inputBufferFirstHalf = ABUF_audioBufferPtrs[config->channelIndex].leftRxFirstHalf;
		node->data.input->inputBufferSecHalf = ABUF_audioBufferPtrs[config->channelIndex].leftRxSecHalf;
	}
	else
	{
		node->data.input->inputBufferFirstHalf = ABUF_audioBufferPtrs[config->channelIndex].rightRxFirstHalf;
		node->data.input->inputBufferSecHalf = ABUF_audioBufferPtrs[config->channelIndex].rightRxSecHalf;
	}

	for (size_t i = 0; i < NODE_MAX_INPUT_NODES; i++)
	{
		node->pInputNodes[i] = NULL;
	}

	*generatedNode = node;

	return BUFMGR_OK;
}

BUFMGR_status_t INPUT_processNode(NODE_node_t *node)
{
	// Check if the node is valid
	if (node == NULL || node->data.input == NULL)
	{
		return BUFMGR_ERROR;
	}

	volatile q15_t* inputBuffer;
	// Process the input node by copying data from the audio buffer to the input buffer
	if(DSP_bufferHalf == ABUF_FIRST_HALF)
	{
		inputBuffer = node->data.input->inputBufferFirstHalf;
	}
	else
	{
		inputBuffer = node->data.input->inputBufferSecHalf;
	}

	arm_q15_to_float(inputBuffer, node->pOutputBuffer->buffer, AUDIO_BUFFER_LENGTH_HALF);

	return BUFMGR_OK;
}

