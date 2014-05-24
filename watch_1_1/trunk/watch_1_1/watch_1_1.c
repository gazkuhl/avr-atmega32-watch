/*
 * watch_1_0.cpp
 *
 * Created: 29.03.2014 19:58:32
 *  Author: reinhard
 */ 



#define F_CPU 16000000UL
			  
#include <avr/io.h>
#include <util/delay.h>
#include "avr/interrupt.h"

#include "header/watch_structs.h"
#include "header/bitio.h"

void takt();
void output_set(int number, uint8_t *output);
void watch_init();



time_t main_time;

int main(void)
{
	
	
	
	main_time.hours_l = 0;
	main_time.hours_l ++;
	main_time.seconds_l = 0;
	
	void port_init();
	PORTA &= ~((1 << PA7));
	PORTA |= ((1 << PA1));
	_delay_ms(10);
	BIT_SET(&PORTA, PA7);
	watch_init();
	BIT_SET(&PORTA, PA6);
	PORTA &= ~((1 << PA1)); //reset
	
    while(1)
    {
		
		main_time.seconds_l ++;
		if(main_time.seconds_l > 9)
		{
			main_time.seconds_l = 0;
			main_time.seconds_h++;
			if (main_time.seconds_h > 5)
			{
				main_time.seconds_h = 0;
				main_time.minutes_l++;
			}
		}
		if ((PIND & (1<<PIND2)))
		{
			main_time.seconds_l = -1;
		}
		
		_delay_ms(1000);
		
		

		
    }
}


void takt()
{
	PORTA |= ((1 << 1));
	_delay_ms(1);
	PORTA &= ~((1 << 1));
}
	
	
void watch_init()
{

		DDRA  = 0xFF;			//DDRA set to output
		PORTA = 0x0;
		DDRB = 0xff;
		PORTB = 0x0;
		DDRC = 0xff;
		DDRD = 0xf3;			//PD2,PD3 input
		
		ASSR = 0x08;			// asynchronous mode for timer2
		
		
		TCCR0 |= (1 << CS01) | (1 << CS00) | (1 << COM00);			// prescaler 64;  COM00 toggle OC0 = PB3
		TCNT0 = 0;
		TIMSK |= (1 << TOIE0);			//interrupt enable 
		sei();				//global interrupt enable
}
 
ISR(TIMER0_OVF_vect)			// ISR kann unterbrochen werden
{
	//BIT_CLEAR(&PORTA, PA6);			//time meassurement start 
	static int pos_7seg = 0;			//position in 7segment
	volatile uint8_t pos_digit = 0;		//0-3 format: 01:23
	static uint8_t output = SEG_0;
	
	// set Data if PB3 is high -> register is written on rising edge  
	
	if ( (PINB & (1<<PINB3)) ) 
	{
		if (pos_7seg == 0)
		{		
			BIT_CLEAR(&PORTA, PA2);
			BIT_SET(&PORTA, PA0);	 //not reset
		}
		if(pos_7seg <= 9)
		{
			if ( !(output & (1<<pos_7seg)) )
			{
				BIT_SET(&PORTA, PA3);
				BIT_SET(&PORTA, PA1);
				BIT_SET(&PORTC, PC0);
				BIT_SET(&PORTD, PD7);
			}
			else
			{
				BIT_CLEAR(&PORTA, PA3);
				BIT_CLEAR(&PORTA, PA1);
				BIT_CLEAR(&PORTC, PC0);
				BIT_CLEAR(&PORTD, PD7);
			}
			if (pos_7seg == 8 )
			{
				BIT_SET(&PORTA, PA2);		//write data to output?!
			}
			if (pos_7seg == 9 )
			{
				BIT_CLEAR(&PORTA, PA0);		//reset
			}
			pos_7seg ++;
		}
		else
		{
			pos_7seg = 0;
			output_set(main_time.seconds_l, &output);
			pos_digit++;
			if(pos_digit > 3) pos_digit = 0;
		}		
	}
		
	//BIT_TOGGLE(&PORTA, PA1);
	//BIT_SET(&PORTA, PA6);	//time meassurement end asd
}


void output_set(int number, uint8_t* output)
{
	switch (number)
	{
	case 0 : *output = SEG_0;			
			break;
	case 1 : *output = SEG_1;
			break;
	case 2 : *output = SEG_2;
			break;
	case 3 : *output = SEG_3;
			break;
	case 4 : *output = SEG_4;
			break;
	case 5 : *output = SEG_5;
			break;
	case 6 : *output = SEG_6;
			break;
	case 7 : *output = SEG_7;
			break;			
	case 8 : *output = SEG_8;
			break;		
	case 9 : *output = SEG_9;
			break;
	default : *output = SEG_ERROR;
			break;			
	}	
}