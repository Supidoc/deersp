/*
 * graph_manager.h
 *
 *  Created on: Aug 13, 2025
 *      Author: dgrob
 */

#ifndef INC_GRAPH_MANAGER_H_
#define INC_GRAPH_MANAGER_H_

#include <stdint.h>
#include "node_manager.h"

#include "biquad_node.h"
#include "mix_node.h"
#include "input_node.h"
#include "output_node.h"

typedef enum _GRPMGR_status_t
{
	GRPMGR_OK, GRPMGR_ERROR,
} GRPMGR_status_t;

#define INPUT_NODE_COUNT 	AUDIO_CHANNEL_COUNT
#define OUTPUT_NODE_COUNT	AUDIO_CHANNEL_COUNT

GRPMGR_status_t GRPMGR_GenerateExampleGraph(void);
GRPMGR_status_t GRPMGR_processGraph(void);
GRPMGR_status_t GRPMGR_linkNodes(uint16_t childNodeId, uint16_t parentNodeId,
		uint8_t inputNodeIndex);
#endif /* INC_GRAPH_MANAGER_H_ */
