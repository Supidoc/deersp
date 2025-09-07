/*
 * logger.h
 *
 *  Created on: Sep 5, 2025
 *      Author: dgrob
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#define LOG_QUEUE_LEN 32
#define LOG_MSG_LEN   128

typedef struct _LOG_item{
    char msg[LOG_MSG_LEN];
} LOG_item_t;

void LOG_task(void *argument);
void LOG_initQueue(void);
void LOG_printf(const char *fmt, ...);

#endif /* INC_LOGGER_H_ */
