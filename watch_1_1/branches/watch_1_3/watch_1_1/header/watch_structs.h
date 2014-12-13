/*
 * IncFile1.h
 *
 * Created: 04.04.2014 22:02:40
 *  Author: gazkuhl
 */ 


#ifndef WATCH_STRUCTS_H_
#define WATCH_STRUCTS_H_

/*defines PortA*/

#define PA7     7
#define PA6     6
#define PA5     5
#define PA4     4
#define PA3     3
#define PA2     2
#define PA1     1
#define PA0     0

/*defines for 7-segment output*/

#define SEG_NO		0xff
#define SEG_0		0x08
#define SEG_1		0x6e
#define SEG_2		0x14
#define SEG_3		0x44
#define SEG_4		0x62
#define SEG_5		0x41
#define SEG_6		0x01
#define SEG_7		0x6c
#define SEG_8		0x0
#define SEG_9		0x40
#define SEG_X		0x22
#define SEG_ERROR	0x11


typedef struct time_s 
{
	uint8_t seconds_l;
	uint8_t seconds_h;
	uint8_t minutes_l;
	uint8_t minutes_h;	
	uint8_t hours_l;
	uint8_t hours_h;
} time_t;





#endif /* INCFILE1_H_ */