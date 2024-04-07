#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "rtp.h"

int server_udp_socket(int port);

int main(int argc, char *argv[])
{
	
	int ret, socket;
	int server_port = 19992;
	int client_port = 0;
	int client_addrlen = 16;
	char client_ipaddr[client_addrlen];
	int rtp_packet_max_size = 65536;
	rtp_packet_t *rtp_packet_ptr;

	socket = server_udp_socket(server_port);
	if(socket < 0) return 1;
	
	rtp_packet_ptr = calloc(rtp_packet_max_size, sizeof(uint8_t));
	if(rtp_packet_ptr == NULL)
	{
		printf("RTP packet allocate error!\n");
		return 1;
	}
	ret = rtp_receive_packet(socket, client_ipaddr, &client_port, rtp_packet_ptr, rtp_packet_max_size);
	if(ret < 0) return 1;
	rtp_packet_ptr->payload[ret - RTP_HEADER_SIZE] = '\0';
	printf("v = %d, p = %d, x = %d, cc = %d, m = %d, pt = %d, seq = %d, ts = %d, ssrc = %d\n",
			  rtp_header_get_v(&rtp_packet_ptr->header),
			  rtp_header_get_p(&rtp_packet_ptr->header),
			  rtp_header_get_x(&rtp_packet_ptr->header),
			  rtp_header_get_cc(&rtp_packet_ptr->header),
			  rtp_header_get_m(&rtp_packet_ptr->header),
			  rtp_header_get_pt(&rtp_packet_ptr->header),
			  rtp_header_get_seq(&rtp_packet_ptr->header),
			  rtp_header_get_ts(&rtp_packet_ptr->header),
			  rtp_header_get_ssrc(&rtp_packet_ptr->header)
			);
	printf("ipaddr = %s, port = %d, payload = %s\n", client_ipaddr, client_port, rtp_packet_ptr->payload);
	free(rtp_packet_ptr);
	
	rtp_packet_ptr = calloc(rtp_packet_max_size, sizeof(uint8_t));
	if(rtp_packet_ptr == NULL)
	{
		printf("RTP packet allocate error!\n");
		return 1;
	}
	ret = rtp_receive_packet(socket, client_ipaddr, &client_port, rtp_packet_ptr, rtp_packet_max_size);
	if(ret < 0) return 1;
	rtp_packet_ptr->payload[ret - RTP_HEADER_SIZE] = '\0';	
	printf("v = %d, p = %d, x = %d, cc = %d, m = %d, pt = %d, seq = %d, ts = %d, ssrc = %d\n",
			  rtp_header_get_v(&rtp_packet_ptr->header),
			  rtp_header_get_p(&rtp_packet_ptr->header),
			  rtp_header_get_x(&rtp_packet_ptr->header),
			  rtp_header_get_cc(&rtp_packet_ptr->header),
			  rtp_header_get_m(&rtp_packet_ptr->header),
			  rtp_header_get_pt(&rtp_packet_ptr->header),
			  rtp_header_get_seq(&rtp_packet_ptr->header),
			  rtp_header_get_ts(&rtp_packet_ptr->header),
			  rtp_header_get_ssrc(&rtp_packet_ptr->header)
			);
	printf("ipaddr = %s, port = %d, payload = %s\n", client_ipaddr, client_port, rtp_packet_ptr->payload);
	free(rtp_packet_ptr);	
	
	return 0;
}

int server_udp_socket(int port)
{
	int n, sockfd;
	struct sockaddr_in servaddr;
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		perror("socket()");
		return -1;
	}
	
	if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind()");
		return -1;
	}
	
	return sockfd;	
}


