#include <string.h>
#include "rtp.h"

void rtp_header_set_v(uint8_t v, rtp_header_t *rtp_header)
{
	uint8_t temp = v << 6; 
	rtp_header->vpxcc = temp | (rtp_header->vpxcc & 0x3F);
}

void rtp_header_set_p(uint8_t p, rtp_header_t *rtp_header)
{
	uint8_t temp = p << 5;
	rtp_header->vpxcc = temp | (rtp_header->vpxcc & 0xDF);
}

void rtp_header_set_x(uint8_t x, rtp_header_t *rtp_header)
{
	uint8_t temp = x << 4;
	rtp_header->vpxcc = temp | (rtp_header->vpxcc & 0xEF);
}

void rtp_header_set_cc(uint8_t cc, rtp_header_t *rtp_header)
{
	rtp_header->vpxcc = cc | (rtp_header->vpxcc & 0xF0);
}

void rtp_header_set_m(uint8_t m, rtp_header_t *rtp_header)
{
	uint8_t temp = m << 7;
	rtp_header->mpt = temp | (rtp_header->mpt & 0x7F);
}

void rtp_header_set_pt(uint8_t pt, rtp_header_t *rtp_header)
{
	rtp_header->mpt = pt | (rtp_header->mpt & 0x80);
}

void rtp_header_set_seq(uint16_t seq, rtp_header_t *rtp_header)
{
	rtp_header->seq = seq;
}

void rtp_header_set_ts(uint32_t ts, rtp_header_t *rtp_header)
{
	rtp_header->ts = ts;
}

void rtp_header_set_ssrc(uint32_t ssrc, rtp_header_t *rtp_header)
{
	rtp_header->ssrc = ssrc;
}