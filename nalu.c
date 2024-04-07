#include <stdlib.h>
#include "nalu.h"

bool startcode3(uint8_t *buf)
{
	/* Поиск в видео-файле кодека h264 стартового кода длиной 3 байта */
	
	return (buf[0] == 0 && buf[1] == 0 && buf[2] == 1) ? true : false;
}

bool startcode4(uint8_t *buf)
{
	/* Поиск в видео-файле кодека h264 стартового кода длиной 4 байта */

	return (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1) ? true : false;
}

int nalu_count(uint8_t *buf, int bufsize)
{
	/* Подсчёт количества nal_unit в видеофайле кодека h264 */
	
	int count = 0;
	
	for(int i = 0; i < bufsize; ++i)
	{
		if(startcode3(buf + i))
		{
			count++;
			i += 3;
		}
		
		if(startcode4(buf + i))
		{
			count++;
			i += 4;
		}		
	}
	
	return count;
}

uint8_t *nalu_next_position(uint8_t *buf, int bufsize)
{
	/* Поиск позиции в видеофайле кодека h264, с которого начинается новый nal_unit */
	
	for(int i = 0; i < bufsize; ++i)
	{
		if(startcode3(buf + i) || startcode4(buf + i))
			return buf + i;
	}
	
	return NULL;
}

int nalu_startcode_len(uint8_t *buf, nalu_t *nalu_ptr)
{
	/* Опредение длины стартового кода, с которого начинается новый nal_unit */
	
	int startcode_len = 0;
	
	if(startcode3(buf)) startcode_len = 3;
		else 
	if(startcode4(buf)) startcode_len = 4;
		else 
	return -1;
	
	if(nalu_ptr != NULL)
	{
		nalu_ptr->startcode_len = startcode_len;
	}
	
	return startcode_len;
}


int nalu_payload_len(uint8_t *next_pos, uint8_t *pos, nalu_t *nalu_ptr)
{
	/* Опредение размера полезной нагрузки nal_unit без учёта байта-заголовка */

	int payload_len = next_pos - pos - nalu_ptr->startcode_len - 1; 
	
	nalu_ptr->payload_len = payload_len;
	
	return payload_len;
}




