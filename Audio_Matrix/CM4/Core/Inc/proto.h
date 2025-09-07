/*
 * proto.h
 *
 *  Created on: Sep 5, 2025
 *      Author: dgrob
 */

#ifndef INC_PROTO_H_
#define INC_PROTO_H_

#include <stdint.h>
#include <stddef.h>

#define PROTO_HEADER   0xAA55
#define PROTO_VERSION  0x01

#pragma pack(push, 1)   // make sure no padding is inserted

typedef struct _PROTO_header {
    uint16_t header;     // always 0xA5A5
    uint8_t  version;    // protocol version, 0x01
    uint8_t  msg_type;   // command ID
    uint16_t seq_num;    // sequence number
    uint16_t length;     // payload length in bytes
    // followed by payload[length]
    // followed by uint16_t CRC16
} PROTO_header_t;

#pragma pack(pop)

typedef enum _PROTO_msg {
	PROTO_MSG_DEVICE_INFO   = 0x01, // request info
	PROTO_MSG_SET_GAIN      = 0x10,
	PROTO_MSG_SET_BIQUAD    = 0x11,
	PROTO_MSG_GET_METERS    = 0x20, // request
	PROTO_MSG_METERS_REPLY  = 0x21, // response
	PROTO_MSG_PRESET_LOAD   = 0x30,
	PROTO_MSG_HEARTBEAT     = 0xFF
} PROTO_msg_t;

#pragma pack(push, 1)

// SET_GAIN payload
typedef struct _PROTO_payload_set_gain {
    uint16_t channel;
    int32_t  gain_q8_24;   // fixed-point gain
} PROTO_payload_set_gain_t;

// SET_BIQUAD payload
typedef struct _PROTO_payload_set_biquad {
    uint16_t channel;
    uint8_t  section;
    int32_t  coeffs[5];    // b0, b1, b2, a1, a2 (Q1.31 format)
} PROTO_payload_set_biquad_t;

// METERS_REPLY payload
typedef struct _PROTO_payload_meters_reply {
    uint16_t channel;
    int32_t  peak_q8_24;
    int32_t  rms_q8_24;
} PROTO_payload_meters_reply_t;

#pragma pack(pop)

void PROTO_process_packet(uint8_t *buf, size_t len);

#endif /* INC_PROTO_H_ */
