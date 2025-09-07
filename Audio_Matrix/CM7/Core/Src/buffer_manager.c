/*
 * buffer_manager.c
 *
 *  Created on: May 24, 2025
 *      Author: dgrob
 */


#include "buffer_manager.h"
#include <stddef.h>
#include <sys/_stdint.h>

//TODO Check activeRefCount logic for completeness
BUFMGR_nodeBuffer_t BUFMGR_bufferPool[BUFMGR_MAX_BUFFER_COUNT];

BUFMGR_status_t BUFMGR_requestBuffer(BUFMGR_nodeBuffer_t **buffer)
{

	// Check if the node is valid
	if (buffer == NULL)
	{
		return BUFMGR_ERROR;
	}

	// Check if there are free buffers in the pool
	for (size_t i = 0; i < BUFMGR_MAX_BUFFER_COUNT; i++)
	{
		if (BUFMGR_bufferPool[i].isUsed == 0)
		{
			// Found a free buffer, assign it to the node
			*buffer = &BUFMGR_bufferPool[i];

			return BUFMGR_OK;
		}
	}
	return BUFMGR_ERROR;
}

BUFMGR_status_t BUFMGR_freeBuffer(BUFMGR_nodeBuffer_t *buffer)
{

	// Check if the node is valid
	if (buffer == NULL)
	{
		return BUFMGR_ERROR;
	}

	buffer->isUsed = 0;

	return BUFMGR_OK;
}




