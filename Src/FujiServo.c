#include "platform_config.h"

#define SERVO_MAX	1

typedef struct {
    unsigned char	Number;
	unsigned char	FunctionCode;
	unsigned short 	RegAddr;
	unsigned short	RegSize;
	unsigned char	DatSize;
	unsigned long	Data[5];
} reg_wr_conf;

static void covert2arry(uint8_t *arry, reg_wr_conf *dt, uint8_t len)
{
	*arry++ = dt->Number;
	*arry++ = dt->FunctionCode;
	*arry++ = dt->RegAddr>>8;
	*arry++ = dt->RegAddr;
	*arry++ = dt->RegSize>>8;
	*arry++ = dt->RegSize;
	*arry++ = dt->DatSize<<2;
	for(uint8_t i=0; i<dt->DatSize; i++) {
		*arry++ = dt->Data[i]>>24;
		*arry++ = dt->Data[i]>>16;
		*arry++ = dt->Data[i]>>8;
		*arry++ = dt->Data[i];
	}
}

static uint16_t CRCCalculation(uint8_t *pDat, uint8_t len)
{
	uint16_t crc = 0xffff;
	uint16_t poly = 0xa001;
	
	for(uint8_t i=0; i<len; i++){
		crc = crc ^ *pDat++;
		for(uint8_t j=0; j<8; j++){
			if((crc & 0x01) == 0x01){
				crc = crc >> 1;
				crc = crc ^ poly;
			}
			else {
				crc = crc >> 1;
			}
		}
	}
	return crc;
}

static void Send2Driver(uint8_t no, uint8_t DatSize, uint16_t Addr, uint32_t *Dat)
{
	reg_wr_conf reg_conf;
	uint16_t crc;
	uint8_t data[32];

	reg_conf.Number = no;
	reg_conf.FunctionCode = 0x10;
	reg_conf.RegAddr = Addr;
	reg_conf.RegSize = 0x02*DatSize;
	reg_conf.DatSize = DatSize;
	for (uint8_t i=0; i<DatSize; i++){
		reg_conf.Data[i] = *Dat++;
	}
	covert2arry(data, &reg_conf, 7+DatSize*4);
	crc = CRCCalculation(data,7+DatSize*4);
	data[7+DatSize*4] = crc;
	data[7+DatSize*4+1] = crc>>8;
	UART1_SendData(data,7+DatSize*4+2);
}

void ServoOn(uint8_t no, uint8_t onoff)
{
// "01 10 0000 0002 04 00000001 326F"
// "01 10 0000 0002 04 00000000 F3AF"
	uint32_t data = (onoff)? 1:0;
	Send2Driver(no, 1, 0, &data);
}

void ServoOrigin(uint8_t no)
{
// "01 10 0000 0002 04 00000011 33A3"
	uint32_t data = 0x11;
	Send2Driver(no, 1, 0, &data);
}

void ServoPositionReset(uint8_t no)
{
// "01 10 0000 0002 04 00000041 339F"
// "01 10 0000 0002 04 00000001 326F"
	uint32_t data = 0x41;
	Send2Driver(no, 1, 0, &data);
	Delay_us(5);
	data = 0x01;
	Send2Driver(no, 1, 0, &data);
}

void ServoStop(uint8_t no)
{
// "01 10 0000 0002 04 00000021 33B7"
// "01 10 0000 0002 04 00000081 33CF"
	uint32_t data = 0x21;
	Send2Driver(no, 1, 0, &data);
	Delay_us(5);
	data = 0x81;
	Send2Driver(no, 1, 0, &data);
}

void ServoLeft(uint8_t no)
{
// "01 10 4028 0002 04 00002710 DBEE"
// "01 10 0000 0002 04 00000105 323C"
	uint32_t data = 0x2710;
	Send2Driver(no, 1, 0x4028, &data);
	Delay_us(5);
	data = 0x0105;
	Send2Driver(no, 1, 0, &data);
}

void ServoRight(uint8_t no)
{
// "01 10 4028 0002 04 00002710 DBEE"
// "01 10 0000 0002 04 00000105 323C"
	uint32_t data = 0x2710;
	Send2Driver(no, 1, 0x4028, &data);
	Delay_us(5);
	data = 0x0103;
	Send2Driver(no, 1, 0, &data);
}


void ServoMove(uint8_t no, int32_t pos, uint32_t speed, uint32_t acc, uint32_t dec)
{
// "01 10 5100 000A 14 01000000 0007A120 00007530 000007D0 000003E8 0574"
// "01 10 5100 000A 14 01000000 FFF85EE0 00007530 000007D0 000003E8 457A"
// "01 10 0000 0002 04 00000009 33A9"
// "01 10 0000 0002 04 00000001 326F"
	uint32_t data[5];
	data[0] = 0x01000000L;
	data[1] = pos;
	data[2] = speed * 100;
	data[3] = acc * 10;
	data[4] = dec * 10;
	Send2Driver(no, 5, 0x5100, data);
	Delay_ms(10);
	data[0] = 9;
	Send2Driver(no, 1, 0, data);
	Delay_ms(10);
	data[0] = 1;
	Send2Driver(no, 1, 0, data);
}

