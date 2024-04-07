#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "rtp.h"


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
						  )
{
	rtp_header_set_v(v, rtp_header);
	rtp_header_set_p(p, rtp_header);
	rtp_header_set_x(x, rtp_header);
	rtp_header_set_cc(cc, rtp_header);
	rtp_header_set_m(m, rtp_header);
	rtp_header_set_pt(pt, rtp_header);
	rtp_header_set_seq(seq, rtp_header);
	rtp_header_set_ts(ts, rtp_header);
	rtp_header_set_ssrc(ssrc, rtp_header);
}


void rtp_packet_init(
							rtp_packet_t *rtp_packet,
							rtp_header_t *rtp_header, 
						   uint8_t *buf,
						   int bufsize
						  )
{
	rtp_packet->header = *rtp_header;
	memcpy(rtp_packet->payload, buf, bufsize);
}


int rtp_receive_packet(
							  int sockfd,
						     char *ipaddr,
						     int *port,
						     rtp_packet_t *packet,
						     int size
					       )
{
	int n;
	socklen_t len;
	struct sockaddr_in cliaddr;
	
	len = sizeof(cliaddr);
	bzero(&cliaddr, sizeof(cliaddr));

	n = recvfrom(sockfd, (void *)packet, size, 0, (struct sockaddr *) &cliaddr, &len);
	if(n < 0)
	{
		perror("recvfrom()");
		return 1;		
	}
	
	*port = ntohs(cliaddr.sin_port);
	if(inet_ntop(AF_INET, ipaddr, (void *) &cliaddr.sin_addr, 16) <= 0)
	{
		perror("inet_ntop");
		return -1;
	}
				 
	packet->header.seq  = ntohs(packet->header.seq);
	packet->header.ts   = ntohs(packet->header.ts);
	packet->header.ssrc = ntohs(packet->header.ssrc);	

	return n;
}


int rtp_send_packet(
						  int sockfd,
						  const char *ipaddr,
						  int port,
						  rtp_packet_t *packet,
						  int size
					    )
{
	int ret;
	struct sockaddr_in servaddr;
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if(inet_pton(AF_INET, ipaddr, (void *)&servaddr.sin_addr) <= 0)
	{
		perror("inet_pton");
		return -1;
	}

	packet->header.seq  = htons(packet->header.seq);
	packet->header.ts   = htons(packet->header.ts);
	packet->header.ssrc = htons(packet->header.ssrc);

	ret = sendto(
					sockfd, 
					(void*) packet, 
					size,
					0,
					(struct sockaddr*) &servaddr, 
					sizeof(servaddr)
				 );
				 
	packet->header.seq  = ntohs(packet->header.seq);
	packet->header.ts   = ntohs(packet->header.ts);
	packet->header.ssrc = ntohs(packet->header.ssrc);

	return ret;
}