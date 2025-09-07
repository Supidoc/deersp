/*
 * mix_node.h
 *
 *  Created on: May 25, 2025
 *      Author: dgrob
 */

#ifndef INC_MIX_NODE_H_
#define INC_MIX_NODE_H_

#include "arm_math.h"
#include "node_manager.h"

typedef struct _MIX_nodeData
{
} MIX_nodeData_t;

typedef struct _MIX_nodeConfig
{
	uint16_t nodeId;
} MIX_nodeConfig_t;



BUFMGR_status_t MIX_createNode(MIX_nodeConfig_t* config, NODE_node_t **generatedNode);

BUFMGR_status_t MIX_processNode(NODE_node_t *node);

#endif /* INC_MIX_NODE_H_ */
