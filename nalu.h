#include <stdint.h>
#include <stdbool.h>

#ifndef NALU_H
#define NALU_H


typedef struct
{
	int number;        // Порядковый номер nal_unit внутри видео-файла кодека h264
	int startcode_len; // Размер страртвого кода nal_unit
	int forbidden_bit; // Значение защищенного бита байта-заголовка nal_unit
	int reference_idc; // Значение важности nal_unit для декодера	
	int unit_type; 	 // Тип полезной нагрузки nal_unit
	int payload_len;   // Размер полезной нагрузки nal_unit

} nalu_t;


bool startcode4(uint8_t *buf);
bool startcode4(uint8_t *buf);

int nalu_count(uint8_t *buf, int bufsize);
uint8_t *nalu_next_position(uint8_t *buf, int bufsize);
int nalu_startcode_len(uint8_t *buf, nalu_t *nalu_ptr);
int nalu_payload_len(uint8_t *next_pos, uint8_t *pos, nalu_t *nalu_ptr);

int nalu_unit_header(uint8_t *buf);
int nalu_forbidden_bit(int unit_header, nalu_t *nalu_ptr);
int nalu_reference_idc(int unit_header, nalu_t *nalu_ptr);
int nalu_unit_type(int unit_header, nalu_t *nalu_ptr);

uint8_t nalu_fua_get_header(uint8_t s, uint8_t e, uint8_t type);

#endif // NALU_H