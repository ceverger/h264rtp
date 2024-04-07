#include <stdint.h>

#ifndef RTP_H
#define RTP_H

#define RTP_HEADER_SIZE 12

/*
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |V=2|P|X| CC |M| PT | sequence number |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | timestamp |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | synchronization source (SSRC) identifier |
 +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 | contributing source (CSRC) identifiers |
 | .... |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 
 version         (v)    : 2  bits
 padding         (p)    : 1  bit
 extension       (x)    : 1  bit
 CSRC count      (cc)   : 4  bits
 marker          (m)    : 1  bit
 payload type    (pt)   : 7  bits
 sequence number (seq)  : 16 bits
 timestamp       (ts)   : 32 bits
 ssrc            (ssrc) : 32 bits
*/

typedef struct
{
	uint8_t  vpxcc;
	uint8_t  mpt;
	uint16_t seq;
	uint32_t ts;
	uint32_t ssrc;
} rtp_header_t;

typedef struct // Описание структуры rtp пакета
{	
	rtp_header_t header;
	uint8_t *payload;

} rtp_packet_t;


uint8_t rtp_header_get_v(rtp_header_t *rtp_header);
uint8_t rtp_header_get_p(rtp_header_t *rtp_header);
uint8_t rtp_header_get_x(rtp_header_t *rtp_header);
uint8_t rtp_header_get_cc(rtp_header_t *rtp_header);
uint8_t rtp_header_get_m(rtp_header_t *rtp_header);
uint8_t rtp_header_get_pt(rtp_header_t *rtp_header);
uint16_t rtp_header_get_seq(rtp_header_t *rtp_header);
uint32_t rtp_header_get_ts(rtp_header_t *rtp_header);
uint32_t rtp_header_get_ssrc(rtp_header_t *rtp_header);

void rtp_header_set_v(uint8_t v, rtp_header_t *rtp_header);
void rtp_header_set_p(uint8_t p, rtp_header_t *rtp_header);
void rtp_header_set_x(uint8_t x, rtp_header_t *rtp_header);
void rtp_header_set_cc(uint8_t cc, rtp_header_t *rtp_header);
void rtp_header_set_m(uint8_t m, rtp_header_t *rtp_header);
void rtp_header_set_pt(uint8_t pt, rtp_header_t *rtp_header);
void rtp_header_set_seq(uint16_t seq, rtp_header_t *rtp_header);
void rtp_header_set_ts(uint32_t ts, rtp_header_t *rtp_header);
void rtp_header_set_ssrc(uint32_t ssrc, rtp_header_t *rtp_header);

void rtp_header_init(
							rtp_header_t *rtp_header,
							uint8_t v, 
							uint8_t p, 
							uint8_t x, 
							uint8_t cc, 
							uint8_t m, 
							uint8_t pt, 
							uint16_t seq, 
							uint32_t ts, 
							uint32_t ssrc
						  );
		
		
void rtp_packet_init(
							rtp_packet_t *rtp_packet,
							rtp_header_t *rtp_header, 
						   uint8_t *buf,
						   int bufsize
						  );

int rtp_receive_packet(
							  int socket,
						     char *ipaddr,
						     int *port,
						     rtp_packet_t *packet,
						     int size
					       );
						 
int rtp_send_packet(
						  int socket,
						  const char *ipaddr,
						  int port,
						  rtp_packet_t *packet,
						  int size
					    );


#endif // RTP_H


	