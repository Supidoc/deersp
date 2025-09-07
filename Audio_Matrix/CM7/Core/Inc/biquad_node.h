/*
 * biquad_node.h
 *
 *  Created on: May 23, 2025
 *      Author: dgrob
 */

#ifndef INC_BIQUAD_NODE_H_
#define INC_BIQUAD_NODE_H_

#include "arm_math.h"

#include "node_manager.h"

#define BQD_MAX_STAGE_COUNT 5


typedef struct _BQD_nodeData
{
	arm_biquad_casd_df1_inst_f32 instance;
	float32_t state[4*BQD_MAX_STAGE_COUNT];
} BQD_nodeData_t;

typedef struct _BQD_nodeConfig
{
	uint16_t nodeId;
	float32_t coeffs[5];
	uint8_t stageCount;
} BQD_nodeConfig_t;

BUFMGR_status_t BQD_createNode(BQD_nodeConfig_t* config, NODE_node_t **generatedNode);

BUFMGR_status_t BQD_processNode(NODE_node_t* node);

#endif /* INC_BIQUAD_NODE_H_ */
