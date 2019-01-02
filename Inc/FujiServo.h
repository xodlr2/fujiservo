#ifndef __FUJISERVO_H__
#define __FUJISERVO_H__

#include "platform_config.h"

#define SERVO_MTR	1
#define SERVO_ON	1
#define SERVO_OFF	0

void ServoOn(uint8_t no, uint8_t onoff);
void ServoOrigin(uint8_t no);
void ServoPositionReset(uint8_t no);
void ServoStop(uint8_t no);
void ServoLeft(uint8_t no);
void ServoRight(uint8_t no);
void ServoMove(uint8_t no, int32_t pos, uint32_t speed, uint32_t acc, uint32_t dec);



#endif  //__FUJISERVO_H__
