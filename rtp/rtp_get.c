#include "rtp.h"

uint8_t rtp_header_get_v(rtp_header_t *rtp_header)
{
	uint8_t v = rtp_header->vpxcc >> 6;
	return v;
}

uint8_t rtp_header_get_p(rtp_header_t *rtp_header)
{
	uint8_t p = (rtp_header->vpxcc >> 5) & 0x01;
	return p;
}

uint8_t rtp_header_get_x(rtp_header_t *rtp_header)
{
	uint8_t x = (rtp_header->vpxcc >> 4) & 0x01;
	return x;
	
}

uint8_t rtp_header_get_cc(rtp_header_t *rtp_header)
{
	uint8_t cc = rtp_header->vpxcc & 0x0F;
	return cc;
}

uint8_t rtp_header_get_m(rtp_header_t *rtp_header)
{
	uint8_t m = rtp_header->mpt >> 7;
	return m;	
}

uint8_t rtp_header_get_pt(rtp_header_t *rtp_header)
{
	uint8_t cc = rtp_header->mpt & 0x7F;
	return cc;
}

uint16_t rtp_header_get_seq(rtp_header_t *rtp_header)
{
	return rtp_header->seq;
}

uint32_t rtp_header_get_ts(rtp_header_t *rtp_header)
{
	return rtp_header->ts;
}

uint32_t rtp_header_get_ssrc(rtp_header_t *rtp_header)
{	
	return rtp_header->ssrc;
}