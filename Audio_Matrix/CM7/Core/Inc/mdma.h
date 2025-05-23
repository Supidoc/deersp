/*
 * mdma.h
 *
 *  Created on: May 19, 2025
 *      Author: dgrob
 */

#ifndef INC_MDMA_H_
#define INC_MDMA_H_

#include "main.h"

void MDMA_init(void);
HAL_StatusTypeDef MDMA_registerRxCallbacks(void (* pXferCallback)(MDMA_HandleTypeDef *_hmdma), void (* pErrorCallback)(MDMA_HandleTypeDef *_hmdma));
HAL_StatusTypeDef MDMA_registerTxCallbacks(void (* pXferCallback)(MDMA_HandleTypeDef *_hmdma), void (* pErrorCallback)(MDMA_HandleTypeDef *_hmdma));

HAL_StatusTypeDef MDMA_startRxTransfer(void);
HAL_StatusTypeDef MDMA_startTxTransfer(void);

uint8_t MDMA_getCurrentRxChIndex(void);
uint8_t MDMA_getCurrentTxChIndex(void);

extern MDMA_HandleTypeDef *MDMA_hmdmaRxInstance;
extern MDMA_HandleTypeDef *MDMA_hmdmaTxInstance;

#endif /* INC_MDMA_H_ */
