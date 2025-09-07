/*
 * node.h
 *
 *  Created on: Aug 28, 2025
 *      Author: dgrob
 */

#ifndef INC_NODE_H_
#define INC_NODE_H_

#include "buffer_manager.h"

#define NODE_MAX_INPUT_NODES 10

// Forward declaration of BQD_nodeData_t
typedef struct _BQD_nodeData BQD_nodeData_t;
// Forward declaration of MIX_nodeData_t
typedef struct _MIX_nodeData MIX_nodeData_t;
// Forward declaration of INPUT_nodeData_t
typedef struct _INPUT_nodeData INPUT_nodeData_t;
// Forward declaration of OUPUT_nodeData_t
typedef struct _OUTPUT_nodeData OUPUT_nodeData_t;

typedef enum _NODE_nodeType_t
{
	NODE_EMPTY_NODE = 0,
	NODE_BIQUAD_NODE,
	NODE_MIX_NODE,
	NODE_INPUT_NODE,
	NODE_OUTPUT_NODE,
} NODE_nodeType_t;


typedef struct _NODE_node_t NODE_node_t;
struct _NODE_node_t
{
	uint16_t nodeId;
	NODE_nodeType_t nodeType;
	uint8_t maxRefCount;
	uint8_t currentRefCount;
	BUFMGR_nodeBuffer_t *pOutputBuffer;
	NODE_node_t *pInputNodes[NODE_MAX_INPUT_NODES];
	uint8_t inputNodeCount;
	bool isProcessed;
	union
	{
		BQD_nodeData_t *biquad;
		MIX_nodeData_t *mix;
		INPUT_nodeData_t *input;
		OUPUT_nodeData_t *output;
	} data;
};

#endif /* INC_NODE_H_ */
