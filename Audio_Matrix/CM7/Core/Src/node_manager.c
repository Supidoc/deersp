/*
 * node_manager.c
 *
 *  Created on: Aug 28, 2025
 *      Author: dgrob
 */

#include "node_manager.h"

NODE_node_t NDMGR_nodePool[NDMGR_MAX_NODE_COUNT] = {{.nodeType = 0 }};

BUFMGR_status_t NDMGR_freeNodeBuffer(NODE_node_t *node)
{

	// Check if the node is valid
	if (node == NULL)
	{
		return BUFMGR_ERROR;
	}

	if(node->maxRefCount > 0)
	{
		return BUFMGR_ERROR; // Cannot free buffer if still referenced
	}

	BUFMGR_status_t status = BUFMGR_freeBuffer(node->pOutputBuffer);

	if(status != BUFMGR_OK)
	{
		return status; // Return if freeing failed
	}

	node->pOutputBuffer = NULL;


	return BUFMGR_OK;
}

BUFMGR_status_t NDMGR_processNode(NODE_node_t *node)
{
	// Check if the node is valid
	if (node == NULL || node->pOutputBuffer == NULL)
	{
		return BUFMGR_ERROR;
	}

	BUFMGR_status_t status = BUFMGR_OK;

	if (node->isProcessed == false)
	{
		for (size_t i = 0; i < node->inputNodeCount; i++)
		{
			NDMGR_processNode(node->pInputNodes[i]);
		}

		if (node->pOutputBuffer == NULL)
		{
			BUFMGR_requestBuffer(&node->pOutputBuffer);

			node->currentRefCount = node->maxRefCount;
		}



		// Process the node based on its type
		switch (node->nodeType)
		{
		case NODE_BIQUAD_NODE:
			status = BQD_processNode(node);
		case NODE_MIX_NODE:
			status = MIX_processNode(node);
			break;
		case NODE_INPUT_NODE:
			status = INPUT_processNode(node);
		default:
			return BUFMGR_ERROR;
		}
	}

	if (status != BUFMGR_OK)
	{
		return status; // Return if processing failed
	}

	node->isProcessed = true; // Mark the node as processed

	for (size_t i = 0; i < node->inputNodeCount; i++)
	{
		if (node->pInputNodes[i] != NULL)
		{
			node->pInputNodes[i]->currentRefCount--;
			if (node->pInputNodes[i]->currentRefCount == 0)
			{
				// If the input buffer is no longer referenced, free it
				BUFMGR_freeBuffer(node->pInputNodes[i]->pOutputBuffer);
				node->pInputNodes[i]->pOutputBuffer = NULL;
			}
		}

	}

	return BUFMGR_OK;
}

BUFMGR_status_t NDMGR_requestNodeSlot(NODE_node_t **nodePointer)
{

	// Check if the node is valid
	if (nodePointer == NULL)
	{
		return BUFMGR_ERROR;
	}

	// Check if there are free buffers in the pool
	for (size_t i = 0; i < NDMGR_MAX_NODE_COUNT; i++)
	{
		if (NDMGR_nodePool[i].nodeType == NODE_EMPTY_NODE)
		{
			// Found a free buffer, assign it to the node
			*nodePointer = &NDMGR_nodePool[i];

			return BUFMGR_OK;
		}
	}
	return BUFMGR_ERROR;
}
