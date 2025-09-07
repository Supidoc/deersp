#include "logger.h"
#include "stm32h7xx_hal.h"

#include "cmsis_os.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define ASCII_ESC 27

static osMessageQueueId_t log_queue;

void LOG_task(void *argument)
{
	LOG_item_t item;

	UART_HandleTypeDef *huart = (UART_HandleTypeDef *)argument;

	// Clear screen
	snprintf(item.msg,LOG_MSG_LEN, "%c[2J", ASCII_ESC );
	HAL_UART_Transmit(huart, (uint8_t *)item.msg, strlen(item.msg), HAL_MAX_DELAY);

	while (1)
	{
		if (osMessageQueueGet(log_queue, &item, NULL, 0) == osOK)
		{
			// TODO Backend: send to UART, ITM, or save to flash
			// OR rtt_write(item.msg);
			// OR sdcard_write_log(item.msg);
			HAL_UART_Transmit(huart, (uint8_t *)item.msg, strlen(item.msg), HAL_MAX_DELAY);

		}
		osDelay(1);
	}
}

void LOG_initQueue(void)
{
	log_queue = osMessageQueueNew(LOG_QUEUE_LEN, sizeof(LOG_item_t), NULL);
}

void LOG_printf(const char *fmt, ...)
{
	LOG_item_t item;
	va_list args;
	va_start(args, fmt);
	vsnprintf(item.msg, LOG_MSG_LEN, fmt, args);
	va_end(args);
	osMessageQueuePut(log_queue, &item, 0, 0);
}
