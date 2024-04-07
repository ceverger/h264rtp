#include <stdio.h>
#include "rtp.h"

int main(int argc, char *argv[])
{
	rtp_header_t header;
	
	rtp_header_set_v(2, &header);
	rtp_header_set_p(0, &header);
	rtp_header_set_x(0, &header);
	rtp_header_set_cc(0, &header);
	rtp_header_set_m(0, &header);
	rtp_header_set_pt(28, &header);
	rtp_header_set_seq(24, &header);
	rtp_header_set_ts(3600, &header);
	rtp_header_set_ssrc(0x90876481, &header);

	printf(
			 "v = %d, p = %d, x = %d, cc = %d, m = %d, pt = %d, seq = %d, ts = %d, ssrc = %x\n",
			  rtp_header_get_v(&header),
			  rtp_header_get_p(&header),
			  rtp_header_get_x(&header),
			  rtp_header_get_cc(&header),
			  rtp_header_get_m(&header),
			  rtp_header_get_pt(&header),
			  rtp_header_get_seq(&header),
			  rtp_header_get_ts(&header),
			  rtp_header_get_ssrc(&header)
			);
			
	rtp_header_init(&header, 1, 1, 1, 7, 1, 23, 16, 8000, 12345);
	
	printf(
			 "v = %d, p = %d, x = %d, cc = %d, m = %d, pt = %d, seq = %d, ts = %d, ssrc = %d\n",
			  rtp_header_get_v(&header),
			  rtp_header_get_p(&header),
			  rtp_header_get_x(&header),
			  rtp_header_get_cc(&header),
			  rtp_header_get_m(&header),
			  rtp_header_get_pt(&header),
			  rtp_header_get_seq(&header),
			  rtp_header_get_ts(&header),
			  rtp_header_get_ssrc(&header)
			);
			
			printf("size_header = %ld\n", sizeof(header));
			
	return 0;
}