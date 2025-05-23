/*
 * dsp.h
 *
 *  Created on: May 22, 2025
 *      Author: dgrob
 */

#ifndef INC_DSP_H_
#define INC_DSP_H_

#include "arm_math.h"


arm_status DSP_init(void);
void DSP_process(void);


#endif /* INC_DSP_H_ */
