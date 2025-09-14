/*
 * amp.c
 *
 *  Created on: Sep 6, 2025
 *      Author: dgrob
 */

#include "amp_m4.h"
#include "openamp.h"
#include "logger.h"
#include "cmsis_os.h"

static volatile int message_received;
volatile char *received_data_str;
static struct rpmsg_endpoint rp_endpoint;

static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data,
		size_t len, uint32_t src, void *priv);
unsigned int receive_message(void);

void AMP_Task(void *argument)
{

	int32_t status = 0;

	/* Initialize the mailbox use notify the other core on new message */
	MAILBOX_Init();

	/* Initialize OpenAmp and libmetal libraries */
	if (MX_OPENAMP_Init(RPMSG_REMOTE, NULL) != HAL_OK)
	{
		LOG_printf("Failed to initialize OpenAMP\n");
		osThreadExit();
	}

	/* Create an endpoint for rmpsg communication */
	status = OPENAMP_create_endpoint(&rp_endpoint, RPMSG_CHAN_NAME,
			RPMSG_ADDR_ANY, rpmsg_recv_callback, NULL);
	if (status < 0)
	{
		__BKPT(1);
		LOG_printf("Failed to create rpmsg endpoint\n");
		osThreadExit();
	}

	LOG_printf("AMP Task initialized and endpoint created.\r\n");

	/* Infinite loop */
	for (;;)
	{
		if(message_received)
		{
			LOG_printf("AMP received: %s\r\n", received_data_str);
			message_received=0;
		}
		else
		{
			OPENAMP_check_for_message();
		}

		osDelay(1);
	}
}


static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data,
               size_t len, uint32_t src, void *priv)
{
  received_data_str = (char *) data;
  message_received=1;

  return 0;
}
