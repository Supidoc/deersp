/*
 * biquad_node.c
 *
 *  Created on: May 23, 2025
 *      Author: dgrob
 */


#include "biquad_node.h"

BUFMGR_status_t BQD_createNode(BUFMGR_node_t* node, BQD_nodeConfig_t config, BUFMGR_node_t* inputNode){
	node->nodeType = BUFMGR_BIQUAD_NODE;
	node->data.biquad->inputNode = inputNode;
	BUFMGR_createReference(node, inputNode);
	arm_biquad_cascade_df1_init_f32(&node->data.biquad->instance, config.stageCount, config.coeffs, node->data.biquad->state);
	return BUFMGR_OK;
}
