/*
 * amp_m7.h
 *
 *  Created on: Sep 6, 2025
 *      Author: dgrob
 */

#ifndef INC_AMP_M7_H_
#define INC_AMP_M7_H_

#include <stdbool.h>
#include "stddef.h"

#define RPMSG_CHAN_NAME "m7-to-m4"

void AMP_init(void);

void AMP_sendMessage(char *msg, size_t len);

extern volatile int service_created;

#endif /* INC_AMP_M7_H_ */
