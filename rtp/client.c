#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "rtp.h"

#define str1 "Hello, world!"
#define str2 "Goodby, world!"

int client_udp_socket();

int main(int argc, char *argv[])
{
	int socket, ret;
	int port = 19992;
	int packet_size = 0;
	const char *ipaddr = "127.0.0.1";
	
	char buf1[strlen(str1) + 1];
	char buf2[strlen(str2) + 1];
	
	rtp_header_t rtp_header;
	rtp_packet_t *rtp_packet_ptr;
	
	socket = client_udp_socket(ipaddr, port);
	if(socket < 0) return 1;
	
	for(int i = 0; i < strlen(str1) + 1; ++i)
	{
		buf1[i] = str1[i];
	}	
	for(int i = 0; i < strlen(str2) + 1; ++i)
	{
		buf2[i] = str2[i];
	}
	
	rtp_header_init(&rtp_header, 0, 0, 0, 0, 0, 28, 0, 0, 56892);

	packet_size = sizeof(rtp_header) + strlen(str1) + 1;
	rtp_packet_ptr = calloc(packet_size, sizeof(uint8_t));
	if(rtp_packet_ptr == NULL)
	{
		printf("RTP packet allocate error!\n");
		return 1;
	}
	rtp_packet_init(rtp_packet_ptr, &rtp_header, buf1, strlen(str1) + 1);
	ret = rtp_send_packet(socket, ipaddr, port,rtp_packet_ptr, packet_size);
	if(ret < 0) return 1;
	free(rtp_packet_ptr);
	
	packet_size = sizeof(rtp_header) + strlen(str2) + 1;
	rtp_packet_ptr = calloc(packet_size, sizeof(uint8_t));	
	rtp_packet_init(rtp_packet_ptr, &rtp_header, buf1, strlen(str1) + 1);
	ret = rtp_send_packet(socket, ipaddr, port, rtp_packet_ptr, packet_size);
	if(ret < 0) return 1;	
	
	return 0;
}


int client_udp_socket()
{
	
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	
	return sockfd;
}