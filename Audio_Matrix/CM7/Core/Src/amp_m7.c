/*
 * amp_m7.c
 *
 *  Created on: Sep 6, 2025
 *      Author: dgrob
 */
#include "amp_m7.h"
#include "openamp.h"
#include <stdio.h>
#include "main.h"

char str2cm4[] = "Bare-Metal programming - sending str from CM7 to CM4 core using OpenAMP framework";
static volatile int message_received;
volatile int service_created;
volatile unsigned int received_data_str;
static struct rpmsg_endpoint rp_endpoint;


static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv);

static void service_destroy_cb(struct rpmsg_endpoint *ept);
static void new_service_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest);

void AMP_init(void)
{
	int32_t status = 0;

	 	/* Initialize the mailbox use notify the other core on new message */
	 	MAILBOX_Init();

	 	/* Initialize the rpmsg endpoint to set default addresses to RPMSG_ADDR_ANY */
	 	rpmsg_init_ept(&rp_endpoint, RPMSG_CHAN_NAME, RPMSG_ADDR_ANY, RPMSG_ADDR_ANY, NULL, NULL);

	 	/* Initialize OpenAmp and libmetal libraries */
	 	if (MX_OPENAMP_Init(RPMSG_MASTER, new_service_cb)!= HAL_OK)
	 	{
	 		Error_Handler();
	 	}




	 	/*
	 	* The rpmsg service is initiate by the remote processor, on A7 new_service_cb
	 	* callback is received on service creation. Wait for the callback
	 	*/
	 	OPENAMP_Wait_EndPointready(&rp_endpoint);

}

static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data,
                size_t len, uint32_t src, void *priv)
{
  received_data_str = *((unsigned int *) data);
  message_received=1;

  return 0;
}

void service_destroy_cb(struct rpmsg_endpoint *ept)
{
  /* this function is called while remote endpoint as been destroyed, the
   * service is no more available
   */
  service_created = 0;
}

void new_service_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest)
{
  /* create a endpoint for rmpsg communication */
  OPENAMP_create_endpoint(&rp_endpoint, name, dest, rpmsg_recv_callback,
                          service_destroy_cb);
  service_created = 1;
}

void AMP_sendMessage(char *msg, size_t len)
{
	if (service_created)
	{
		rpmsg_send(&rp_endpoint, (void *) msg, len);
	}
}


