/*
 * proto.c
 *
 *  Created on: Sep 5, 2025
 *      Author: dgrob
 */

#include "proto.h"
#include "lwip/def.h"

#include <string.h>

void PROTO_process_packet(uint8_t *buf, size_t len) {
    if (len < sizeof(PROTO_header_t) + 2) return;

    PROTO_header_t *hdr = (PROTO_header_t*)buf;
    uint16_t header = lwip_ntohs(hdr->header);
    if (header != PROTO_HEADER) return; // bad header

    uint16_t payload_len = lwip_ntohs(hdr->length);
    if (len < sizeof(PROTO_header_t) + payload_len + 2) return; // incomplete

    uint8_t *payload = buf + sizeof(PROTO_header_t);
    uint16_t crc_rx;
    memcpy(&crc_rx, buf + sizeof(PROTO_header_t) + payload_len, 2);
    //TODO verify CRC here...

    switch (hdr->msg_type) {
        case PROTO_MSG_SET_GAIN: {
            PROTO_payload_set_gain_t *pl = (PROTO_payload_set_gain_t*)payload;
            uint16_t ch = lwip_ntohs(pl->channel);
            int32_t gain = lwip_ntohl(pl->gain_q8_24);
            // Forward to M7 via OpenAMP
            break;
        }
        case PROTO_MSG_GET_METERS: {
            // handle request, send back MSG_METERS_REPLY
            break;
        }
        default:
            break;
    }
}
