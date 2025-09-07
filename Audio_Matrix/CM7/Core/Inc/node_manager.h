/*
 * node_manager.h
 *
 *  Created on: Aug 28, 2025
 *      Author: dgrob
 */

#ifndef INC_NODE_MANAGER_H_
#define INC_NODE_MANAGER_H_

#include "buffer_manager.h"
#include "node.h"

#include "biquad_node.h"
#include "mix_node.h"
#include "input_node.h"
#include "output_node.h"

#define NDMGR_MAX_NODE_COUNT 100

extern NODE_node_t NDMGR_nodePool[NDMGR_MAX_NODE_COUNT];

BUFMGR_status_t NDMGR_freeNodeBuffer(NODE_node_t *node);
BUFMGR_status_t NDMGR_processNode(NODE_node_t *node);
BUFMGR_status_t NDMGR_requestNodeSlot(NODE_node_t **nodePointer);



#endif /* INC_NODE_MANAGER_H_ */
