/*
 * biquad_node.h
 *
 *  Created on: May 23, 2025
 *      Author: dgrob
 */

#ifndef INC_BIQUAD_NODE_H_
#define INC_BIQUAD_NODE_H_

#include "arm_math.h"
#include "buffer_manager.h"

#define BQD_MAX_STAGE_COUNT 5

// Forward declaration of BUFMGR_node_t
typedef struct _BUFMGR_node_t BUFMGR_node_t;

typedef struct _BQD_nodeData
{
	arm_biquad_casd_df1_inst_f32 instance;
	float32_t state[4*BQD_MAX_STAGE_COUNT];
	BUFMGR_node_t* inputNode;

} BQD_nodeData_t;

typedef struct _BQD_nodeConfig
{
	float32_t coeffs[5];
	uint8_t stageCount;
} BQD_nodeConfig_t;

BUFMGR_status_t BQD_createNode(BUFMGR_node_t* node, BQD_nodeConfig_t config, BUFMGR_node_t* inputNode);

#endif /* INC_BIQUAD_NODE_H_ */
