/*
 * buffer_manager.c
 *
 *  Created on: May 24, 2025
 *      Author: dgrob
 */

#include "buffer_manager.h"
#include "biquad_node.h"

BUFMGR_nodeBuffer_t BUFMGR_bufferPool[BUFFER_MANAGER_MAX_BUFFER_COUNT];

BUFMGR_status_t BUFMGR_requestBuffer(BUFMGR_node_t* node, BUFMGR_nodeType_t nodeType){

	// Check if the node is valid
	if (node == NULL) {
		return BUFMGR_ERROR;
	}

	// Check if there are free buffers in the pool
	for (size_t i = 0; i < BUFFER_MANAGER_MAX_BUFFER_COUNT; i++) {
		if (BUFMGR_bufferPool[i].refCount == 0) {
			// Found a free buffer, assign it to the node
			node->outputBuffer = &BUFMGR_bufferPool[i];
			node->outputBuffer->refCount = node->outputBuffer->refCount;
			return BUFMGR_OK;
		}
	}
	return BUFMGR_ERROR;
}

BUFMGR_status_t BUFMGR_reduceBufferRefCount(BUFMGR_node_t* node, BUFMGR_nodeType_t nodeType){

	// Check if the node is valid
	if (node == NULL || node->outputBuffer == NULL) {
		return BUFMGR_ERROR;
	}

	// Reduce the reference count of the buffer
	node->outputBuffer->refCount--;

	// If the reference count reaches zero, free the buffer
	if (node->outputBuffer->refCount == 0) {
		node->outputBuffer = NULL;
	}

	return BUFMGR_OK;
}

BUFMGR_status_t BUFMGR_createReference(BUFMGR_node_t* childNode, BUFMGR_node_t* parentNode){

	// Check if the nodes are valid
	if (childNode == NULL || parentNode == NULL) {
		return BUFMGR_ERROR;
	}

	childNode->refCount++;

	return BUFMGR_OK;


}
