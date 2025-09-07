/*
 * output_node.c
 *
 *  Created on: Aug 9, 2025
 *      Author: dgrob
 */

#include "output_node.h"
#include "dsp.h"

BUFMGR_status_t OUTPUT_createNode(OUPUT_nodeConfig_t* config, NODE_node_t **generatedNode)
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
	node->nodeType = NODE_OUTPUT_NODE;
	node->maxRefCount = 0;
	node->isProcessed = false;
	if(config->channelSide == ABUF_CHANNEL_LEFT)
	{
		node->data.output->outputBufferFirstHalf = ABUF_audioBufferPtrs[config->channelIndex].leftTxFirstHalf;
		node->data.output->outputBufferSecHalf = ABUF_audioBufferPtrs[config->channelIndex].leftTxSecHalf;
	}
	else
	{
		node->data.output->outputBufferFirstHalf = ABUF_audioBufferPtrs[config->channelIndex].rightTxFirstHalf;
		node->data.output->outputBufferSecHalf = ABUF_audioBufferPtrs[config->channelIndex].rightTxSecHalf;
	}

	for (size_t i = 0; i < NODE_MAX_INPUT_NODES; i++)
	{
		node->pInputNodes[i] = NULL;
	}

	*generatedNode = node;

	return BUFMGR_OK;
}

BUFMGR_status_t OUTPUT_processNode(NODE_node_t *node)
{
	// Check if the node is valid
	if (node == NULL || node->data.input == NULL)
	{
		return BUFMGR_ERROR;
	}

	volatile float* inputBuffer = node->pInputNodes[0]->pOutputBuffer->buffer;
	volatile q15_t* outputBuffer;
	// Process the output node by copying data from the output buffer to the audio buffer
	if(DSP_bufferHalf == ABUF_FIRST_HALF)
	{
		outputBuffer = node->data.output->outputBufferFirstHalf;
	}
	else
	{
		outputBuffer = node->data.output->outputBufferSecHalf;
	}

	arm_float_to_q15(inputBuffer,outputBuffer, AUDIO_BUFFER_LENGTH_HALF);

	return BUFMGR_OK;
}
