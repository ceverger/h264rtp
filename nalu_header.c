#include <stdlib.h>
#include "nalu.h"

int nalu_unit_header(uint8_t *buf)
{	
	/* Получение значения байта заголовка nalu_unit */
	
	return buf[0] > 0 ? buf[0] : -1;
}

int nalu_forbidden_bit(int unit_header, nalu_t *nalu_ptr)
{	
	/* Получение значения защищённого бита заголовка nalu_unit */
	
	int forbidden_bit = (unit_header & 0x80) >> 7;
	
	if(nalu_ptr != NULL)
	{
		nalu_ptr->forbidden_bit = forbidden_bit;
	}
	
	return forbidden_bit;
}

int nalu_reference_idc(int unit_header, nalu_t *nalu_ptr)
{	
	/* Получение значения важности nalu_unit */
	
	int reference_idc = (unit_header & 0x60) >> 5;

	if(nalu_ptr != NULL)
	{
		nalu_ptr->reference_idc = reference_idc;
	}
	
	return reference_idc;
}

int nalu_unit_type(int unit_header, nalu_t *nalu_ptr)
{
	/* Получение значения типа nalu_unit */
	
	int unit_type = unit_header & 0x1F;
	
	if(nalu_ptr != NULL)
	{
		nalu_ptr->unit_type = unit_type;
	}
	
	return unit_type;
}


uint8_t nalu_fua_get_header(uint8_t s, uint8_t e, uint8_t type)
{
	uint8_t fua_header = 0;
	
	if(s > 0 && e > 0)
	{
		return fua_header;
	}
	
	if(s > 0) 
	{
		s = 1;
		fua_header = s << 7;
	}
	
	if(e > 0)
	{
		e = 1;
		fua_header = e << 6;
	}
	
	fua_header |= type;
	
	return fua_header;
}
