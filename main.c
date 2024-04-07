#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "rtp.h"

static int createUdpSocket(int port);
static int startCode3(char* buf);
static int startCode4(char* buf);
static char *findNextStartCode(char* buf, int len);
static int getFrameFromH264File(int fd, char* frame, int size);
static int rtpSendH264Frame(int socket, char* ip, int16_t port, rtp_packet_t *rtp_packet, uint8_t* frame, uint32_t frameSize);

int main(int argc, char *argv[])
{
	int fd;
	int sockfd;
	int fps = 25;
	int startCode;
	int port = 9832;
	int size = 3000 * 1000;
	char *ipaddr = "127.0.0.1";
	
	uint8_t *frame;
	uint32_t frameSize;
	rtp_packet_t *rtp_packet;

	fd = open("test.264", O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		return 1;
	}
	
	sockfd = createUdpSocket(port);
	if(sockfd < 0)
	{
		perror("socket");
		return 1;
	}
	
	rtp_packet = (rtp_packet_t *) calloc(1400, sizeof(uint8_t));
	
	frame = (uint8_t*) calloc(size);
	
	rtpHeaderInit(&rtp_packet->rtp_header, 0, 0, 0, RTP_VESION, RTP_PAYLOAD_TYPE_H264, 0, 0, 0, 0x88923423);
	
	while(1)
	{
		frameSize = getFrameFromH264File(fd, frame, s);
		if(frameSize < 0)
		{
			printf("read err\n");
			continue;
		}
 
		startCode = startCode3(frame) ? 3 : 4;
 
		frameSize -= startCode;
		rtpSendH264Frame(sockfd, ipaddr, port, rtp_packet, frame + startCode, frameSize);
		rtp_packet->rtp_header.ts += 90000 / fps;
	 
		usleep(1000 * 1000 / fps);
    }
 
    free(rtp_packet);
    free(frame);
	 
	 return 0;
}


int createUdpSocket(int port)
{
	int fd;
	int on = 1;
 
	fd = socket(AF_INET, SOCK_DGRAM, 0);	
	
	if(fd < 0) return -1;	
	
   setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
	
   return fd;
}

static int startCode3(char* buf)
{
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        return 1;
    else
        return 0;
}
 
static int startCode4(char* buf)
{
    if(buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1)
        return 1;
    else
        return 0;
}


static char* findNextStartCode(char* buf, int len)
{
    int i;
 
    if(len < 3) return NULL;
 
    for(i = 0; i < len - 3; ++i)
    {
        if(startCode3(buf) || startCode4(buf))
            return buf;
        
        ++buf;
    }
 
    if(startCode3(buf))
        return buf;
 
    return NULL;
}


static int getFrameFromH264File(int fd, char* frame, int size)
{
    int rSize, frameSize;
    char* nextStartCode;
 
    if(fd < 0) return fd;
 
    rSize = read(fd, frame, size); // Считывание содержимого файла test.264
	 
	 // Поиск стартового кода в начале файла 
    if(!startCode3(frame) && !startCode4(frame))
        return -1;
    
	 // 
    nextStartCode = findNextStartCode(frame + 3, rSize - 3);
    if(!nextStartCode)
    {
        lseek(fd, 0, SEEK_SET);
        frameSize = rSize;
    }
    else
    {
        frameSize = (nextStartCode - frame);
        lseek(fd, frameSize - rSize, SEEK_CUR);
    }
 
    return frameSize;
}



static int rtpSendH264Frame(
                             int socket, 
                             char* ip, 
									  int16_t port,
                             rtp_packet_t *rtp_packet, 
									  uint8_t* frame, 
									  uint32_t frameSize
									)
{
	uint8_t naluType; // первый байт nalu
	int sendBytes = 0;
	int ret;

	naluType = frame[0];

	if (frameSize <= RTP_MAX_PKT_SIZE) // длина nalu меньше максимального поля пакета: режим одиночного блока NALU
	{
		/*
		*   0 1 2 3 4 5 6 7 8 9
		*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*  |F|NRI|  Type   | a single NAL unit ... |
		*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*/
		
		memcpy(rtp_packet->payload, frame, frameSize);
		ret = rtpSendPacket(socket, ip, port, rtp_packet, frameSize);
		if(ret < 0) return -1;
 
		rtp_packet->rtp_header.seq++;
		sendBytes += ret;
      if ((naluType & 0x1F) == 7 || (naluType & 0x1F) == 8)
		{
			// Если это SPS, PPS, отметка времени не требуется
			goto out;
		}
	}
	else // длина nalu меньше максимального поля пакета: режим фрагментации
	{
		/*
		*  0                   1                   2
		*  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
		* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		* | FU indicator  |   FU header   |   FU payload   ...  |
		* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		*/

		/*
		*     FU Indicator
		*    0 1 2 3 4 5 6 7
		*   +-+-+-+-+-+-+-+-+
		*   |F|NRI|  Type   |
		*   +---------------+
		*/

		/*
		*      FU Header
		*    0 1 2 3 4 5 6 7
		*   +-+-+-+-+-+-+-+-+
		*   |S|E|R|  Type   |
		*   +---------------+
		*/
 
		int pktNum = frameSize / RTP_MAX_PKT_SIZE; // Есть несколько полных пакетов
		int stayPktSize = frameSize % RTP_MAX_PKT_SIZE; // Размер оставшихся неполных пакетов
		int i, pos = 1;
 
      /* Отправить полный пакет */
		for (i = 0; i < pktNum; i++)
		{
			rtp_packet->payload[0] = (naluType & 0x60) | 28;
			rtp_packet->payload[1] = naluType & 0x1F;
			
			if(i == 0) // Первый пакет данных
				rtp_packet->payload[1] |= 0x80; // start
			else if (stayPktSize == 0 && i == pktNum-1) // Последний пакет данных
				rtp_packet->payload[1] |= 0x40; // end

			memcpy(rtp_packet->payload + 2, frame + pos, RTP_MAX_PKT_SIZE);
			ret = rtpSendPacket(socket, ip, port, rtp_packet, RTP_MAX_PKT_SIZE + 2);
			
			if(ret < 0) return -1;

			rtp_packet->rtp_header.seq++;
			sendBytes += ret;
			pos += RTP_MAX_PKT_SIZE;
		}
 
		/* Отправляем оставшиеся данные */
		if (ret > 0)
		{
			rtp_packet->payload[0] = (naluType & 0x60) | 28;
			rtp_packet->payload[1] = naluType & 0x1F;
			rtp_packet->payload[1] |= 0x40; //end

			memcpy(rtp_packet->payload + 2, frame + pos, ret);
			ret = rtpSendPacket(socket, ip, port, rtp_packet, 2);
			
			if(ret < 0) return -1;

			rtp_packet->rtp_header.seq++;
			sendBytes += ret;
		}
	}
 
	out:
	
		return sendBytes;
}
