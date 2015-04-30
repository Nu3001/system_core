/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/** ubcsp.h                                                                 **/
/**                                                                         **/
/** MicroBCSP - a very low cost implementation of the BCSP protocol         **/
/**                                                                         **/
/*****************************************************************************/
#ifndef _UBCSP_H_
#define _UBCSP_H_


#include "ubcspcfg.h"

/*****************************************************************************/
/**                                                                         **/
/** ubcsp_packet                                                            **/
/**                                                                         **/
/** This is description of a bcsp packet for the upper layer                **/
/**                                                                         **/
/*****************************************************************************/

struct ubcsp_packet
{
	uint8 channel;		/* Which Channel this packet is to/from */
	uint8 reliable;		/* Is this packet reliable */
	uint8 use_crc;		/* Does this packet use CRC data protection */
	uint16 length;		/* What is the length of the payload data */
	uint8 *payload;		/* The payload data itself - size of length */
};

/*****************************************************************************/
/**                                                                         **/
/** ubcsp_configuration                                                     **/
/**                                                                         **/
/** This is the main configuration of the ubcsp engine                      **/
/** All state variables are stored in this structure                        **/
/**                                                                         **/
/*****************************************************************************/

enum ubcsp_link_establishment_state
{
	ubcsp_le_uninitialized,
	ubcsp_le_initialized,
	ubcsp_le_active
};

enum ubcsp_link_establishment_packet
{
	ubcsp_le_sync,
	ubcsp_le_sync_resp,
	ubcsp_le_conf,
	ubcsp_le_conf_resp,
	ubcsp_le_none
};

struct ubcsp_configuration
{
	uint8 link_establishment_state;
	uint8 link_establishment_resp;
	uint8 link_establishment_packet;

	uint8 sequence_number:3;
	uint8 ack_number:3;
	uint8 send_ack;
	struct ubcsp_packet *send_packet;
	struct ubcsp_packet *receive_packet;

	uint8 receive_header_checksum;
	uint8 receive_slip_escape;
	int32 receive_index;

	uint8 send_header_checksum;
#ifdef UBCSP_CRC
	uint8 need_send_crc;
	uint16 send_crc;
#endif
	uint8 send_slip_escape;

	uint8 *send_ptr;
	int32 send_size;
	uint8 *next_send_ptr;
	int32 next_send_size;

	int8 delay;
};

/*****************************************************************************/
/**                                                                         **/
/** ubcsp_poll sets activity from an OR of these flags                      **/
/**                                                                         **/
/*****************************************************************************/

#define UBCSP_PACKET_SENT 0x01
#define UBCSP_PACKET_RECEIVED 0x02
#define UBCSP_PEER_RESET 0x04
#define UBCSP_PACKET_ACK 0x08

/*****************************************************************************/
/**                                                                         **/
/** This is the functional interface for ucbsp                              **/
/**                                                                         **/
/*****************************************************************************/

void ubcsp_initialize (void);
void ubcsp_send_packet (struct ubcsp_packet *send_packet);
void ubcsp_receive_packet (struct ubcsp_packet *receive_packet);
uint8 ubcsp_poll (uint8 *activity);

/*****************************************************************************/
/**                                                                         **/
/** Slip Escape Values                                                      **/
/**                                                                         **/
/*****************************************************************************/

#define SLIP_FRAME 0xC0
#define SLIP_ESCAPE 0xDB
#define SLIP_ESCAPE_FRAME 0xDC
#define SLIP_ESCAPE_ESCAPE 0xDD

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/**                                                                         **/
/** These functions need to be linked into your system                      **/
/**                                                                         **/
/*****************************************************************************/

/*****************************************************************************/
/**                                                                         **/
/** put_uart outputs a single octet over the UART Tx line                   **/
/**                                                                         **/
/*****************************************************************************/

extern void put_uart (uint8);

/*****************************************************************************/
/**                                                                         **/
/** get_uart receives a single octet over the UART Rx line                  **/
/** if no octet is available, then this returns 0                           **/
/*****************************************************************************/
/************************Flaicomm CopyRight 2010.*****************************/ 
/*****************************************************************************/

extern uint8 get_uart (uint8 *);
extern void put_stream (uint8* stream, uint16 length);//eshin add

/*****************************************************************************/
/**                                                                         **/
/** These defines should be changed to your systems concept of 100ms        **/
/**                                                                         **/
/*****************************************************************************/

#define UBCSP_POLL_TIME_IMMEDIATE   0
#define UBCSP_POLL_TIME_DELAY       25

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#endif
