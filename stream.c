#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "rtp.h"
#include "nalu.h"

int createUdpSocket();

int main(int argc, char *argv[])
{
	int fd;
	int ret;
	int socket;
	int bufsize;
	int fps = 25;
	int offset = 0;
	int number = 0;
	uint8_t *buffer;
	struct stat file_info;

	int pos = 0;
	int next_pos = 0;	
	int unit_count = 0;
	
	int mtu = 1400;
	int port = 9832;
	const char *ipaddr = "127.0.0.1";
	
	nalu_t nalu_info;
	rtp_header_t rtp_header;
	rtp_packet_t *rtp_packet;
	
	if(argc < 2)
	{
		printf("no input file\n");
		return 1;
	}
	
	fd = open(argv[1], O_RDONLY);
	
	if(fd < 0)
	{
		perror("open");
		return -1;
	}
		
	if(fstat(fd, &file_info) < 0)
	{
		perror("fstat");
		return 1;
	}
	
	socket = createUdpSocket();
	
	if(socket < 0)
	{
		perror("socket");
		return 1;
	}
	
	bufsize = (int) file_info.st_size;	
	buffer = (uint8_t *) calloc(bufsize, sizeof(uint8_t));
	
	if(buffer == NULL)
	{
		printf("Error of allocate buffer for h264 file!\n");
		return -1;
	}
	
	if(read(fd, buffer, bufsize) < 0)
	{
		perror("read");
		return -1;
	}
	
	rtpHeaderInit(&rtp_header, 2, 0, 0, 0, 0, 96, 0, 0, 0x88923423);
	unit_count = nalu_count(buffer, bufsize);
			
	while(1)
	{				
		pos = nalu_next_position(buffer, bufsize, offset);
		
		if(pos < 0) 
		{
			break;
		}
		
		nalu_info.position = pos;
		
		nalu_info.startcode_len = nalu_startcode_len(buffer, pos);
				
		next_pos = nalu_next_position(buffer, bufsize, offset + 3);
		
		if(next_pos < 0) next_pos = bufsize;
		
		nalu_info.length = next_pos - nalu_info.position;		
		
		offset += nalu_info.length;
		
		nalu_info.forbidden_bit = nalu_forbidden_bit(buffer, nalu_info.position, nalu_info.startcode_len);

		nalu_info.reference_idc = nalu_reference_idc(buffer, nalu_info.position, nalu_info.startcode_len);
		
		nalu_info.unit_type = nalu_unit_type(buffer, nalu_info.position, nalu_info.startcode_len);
		
		/* printf(
				 "number = %d, position = %d, lenght = %d, startcode_len = %d, forbidden_bit = %d, reference_idc = %d, unit_type = %d\n", 
				 number,
				 nalu_info.position, 
				 nalu_info.length,
				 nalu_info.startcode_len,
				 nalu_info.forbidden_bit,
				 nalu_info.reference_idc,
				 nalu_info.unit_type
			   ); */
		
		if(nalu_info.length - nalu_info.startcode_len <= mtu)
		{
			int packet_size = sizeof(rtp_header) + nalu_info.length - nalu_info.startcode_len;
			rtp_packet = calloc(packet_size, sizeof(uint8_t));
			rtpPacketInit(rtp_packet, &rtp_header, buffer + nalu_info.position + nalu_info.startcode_len, nalu_info.length - nalu_info.startcode_len);
			ret = rtpSendPacket(socket, ipaddr, port, rtp_packet, packet_size);
			if(ret < packet_size)
			{
				printf("Rtp packet send error!");
				return 1;
			}
			
			free(rtp_packet);
		}
		else
		{
			int sendBytes = 0;
			int other_size = 0;
			int packet_size = 0;
			int packet_number = 0;
			int payload_size = 0;
			uint8_t fua_indicator, fua_header;
			
			packet_number = (nalu_info.length - nalu_info.startcode_len - 1) / mtu;
			other_size = (nalu_info.length - nalu_info.startcode_len - 1) % mtu;	
			payload_size = sizeof(fua_indicator) + sizeof(fua_header) + mtu;
			packet_size = sizeof(rtp_header) + payload_size;
			rtp_packet = calloc(packet_size, sizeof(uint8_t));
			
			fua_indicator = nalu_fua_indicator(buffer, nalu_info.position, nalu_info.startcode_len);

			for(int i = 0; i < packet_number; ++i)
			{												
				
				if(i == 0)
				{
					fua_header = nalu_fua_header(buffer, nalu_info.position, nalu_info.startcode_len, 0);
				}
				
				if(i > 0 && i < packet_number - 1)
				{
					fua_header = nalu_fua_header(buffer, nalu_info.position, nalu_info.startcode_len, 1);					
				}
				
				if((i == packet_number - 1) && other_size == 0)
				{
					fua_header = nalu_fua_header(buffer, nalu_info.position, nalu_info.startcode_len, 2);					
				}
				
				rtp_packet->rtp_header = rtp_header;
				rtp_packet->payload[0] = fua_indicator;
				rtp_packet->payload[1] = fua_header;				
				memcpy(rtp_packet->payload + 2, buffer + nalu_info.position + nalu_info.startcode_len + 1 + sendBytes, mtu);
				
				sendBytes += mtu;
				
				ret = rtpSendPacket(socket, ipaddr, port, rtp_packet, packet_size);
				if(ret < packet_size)
				{
					printf("Rtp packet send error!");
					return 1;
				}				
			}
			
			free(rtp_packet);
			
			if(other_size > 0)
			{		
				payload_size = sizeof(fua_indicator) + sizeof(fua_header) + other_size;
				packet_size = sizeof(rtp_header) + payload_size;	
				
				fua_header = nalu_fua_header(buffer, nalu_info.position, nalu_info.startcode_len, 2);	
				
				rtp_packet = calloc(packet_size, sizeof(uint8_t));
				rtp_packet->rtp_header = rtp_header;
				rtp_packet->payload[0] = fua_indicator;
				rtp_packet->payload[1] = fua_header;		
				memcpy(rtp_packet->payload + 2, buffer + nalu_info.position + nalu_info.startcode_len + 1 + sendBytes, other_size);
				
				ret = rtpSendPacket(socket, ipaddr, port, rtp_packet, packet_size);
				if(ret < packet_size)
				{
					printf("Rtp packet send error!");
					return 1;
				}

				free(rtp_packet);
			}			
		}
		
		rtp_header.seq++;
		rtp_header.ts = 90000 / fps;
		
		usleep(1000 * 1000 / fps);
		
		number++;

		if(number == unit_count)
		{		
			offset = 0;
			number = 0;
			rtp_header.seq = 0;
			rtp_header.ts = 0;
		}		
	}
	
	return 0;
}


int createUdpSocket()
{
    int fd;
    int on = 1;
 
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0) return -1;
 
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
 
    return fd;
}
