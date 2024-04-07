#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "nalu.h"

int main(int argc, char *argv[])
{
	int fd;
	int bufsize;
	int offset = 0;
	int number = 0;
	uint8_t *buffer;
	struct stat file_info;
	uint8_t *pos, *next_pos;
	
	nalu_t nalu;
	int unit_count = 0;
	int unit_header = 0;
	int frame_count = 0;
	
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
	
	bufsize = (int) file_info.st_size;	
	buffer = (uint8_t *) calloc(bufsize, sizeof(uint8_t));

	printf("h264 file size = %d\n", bufsize);
	
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
	
	unit_count = nalu_count(buffer, bufsize);
	printf("Nalu unit count = %d\n", unit_count);
	
	for(int i = 0; i < unit_count; ++i)
	{
		pos = nalu_next_position(buffer + offset, bufsize - offset);		
		if(pos == NULL) break;		
		nalu_startcode_len(pos, &nalu);
				
		next_pos = nalu_next_position(buffer + nalu.startcode_len + offset, bufsize - nalu.startcode_len - offset);
		if(next_pos == NULL) next_pos = buffer + bufsize;
		nalu_payload_len(next_pos, pos, &nalu);

		unit_header = nalu_unit_header(buffer + offset + nalu.startcode_len);
		nalu_forbidden_bit(unit_header, &nalu);
		nalu_reference_idc(unit_header, &nalu);
		nalu_unit_type(unit_header, &nalu);
		
		printf("number = %d, startcode_len = %d; forbidden_bit = %d, reference_idc = %d, unit_type = %d, payload_len = %d\n", i, nalu.startcode_len, nalu.forbidden_bit, nalu.reference_idc, nalu.unit_type, nalu.payload_len);

		offset += nalu.startcode_len + 1 + nalu.payload_len;
		if(nalu.unit_type == 9) frame_count++;
	}

	printf("frame_count = %d\n", frame_count);
	
	return 0;
}
