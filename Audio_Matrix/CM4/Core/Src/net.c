/*
 * net.c
 *
 *  Created on: Sep 5, 2025
 *      Author: dgrob
 */

#include "net.h"
#include "proto.h"
#include "logger.h"

#include "lwip/api.h"
#include "cmsis_os.h"

#define UDP_PORT 50000

#define RECV_BUF_SIZE 256

void NET_udp_task(void *argument)
{
	struct netconn *conn;
	struct netbuf *buf;
	ip_addr_t *addr;
	unsigned short port;
	err_t err;

	conn = netconn_new(NETCONN_UDP);

	if (!conn)
	{
		LOG_printf("Failed to create UDP netconn\r\n");
		vTaskDelete(NULL);
		return;
	}

	err = netconn_bind(conn, IP_ADDR_ANY, UDP_PORT);
	if (err != ERR_OK)
	{
		LOG_printf("UDP bind failed\r\n");
		netconn_delete(conn);
		vTaskDelete(NULL);
		return;
	}

	LOG_printf("UDP receive task running on port %d\r\n", UDP_PORT);

	while (1)
	{
		buf = NULL;
		err = netconn_recv(conn, &buf);
		if (err == ERR_OK && buf)
		{
			void *data;
			u16_t len;
			netbuf_data(buf, &data, &len);

			// process_packet handles decoding + OpenAMP forwarding
			PROTO_process_packet((uint8_t*) data, len);

			netbuf_delete(buf);
		}
		else
		{
			osDelay(1); // avoid tight loop on errors
		}
		osDelay(1);
	}
}
