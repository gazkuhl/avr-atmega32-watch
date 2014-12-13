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


void output_set(int number, uint8_t *output);
void watch_init();


time_t main_time;
time_t counter_time;
uint8_t set_clock_flag = 1;
uint8_t reset_flag = 1;
uint8_t timer_flag = 0;
uint8_t timergo = 0;

int main(void)
{
	
	main_time.hours_h = 0;
	main_time.hours_l = 5;	
	main_time.minutes_l = 5;
	main_time.seconds_l = 5;
	
	counter_time.hours_h = 0;
	counter_time.hours_l = 0;
	counter_time.minutes_h = 0;
	counter_time.minutes_l = 0;
	counter_time.seconds_l = 0;
	counter_time.seconds_h = 0;
	
	uint8_t push_flag = 0;
	
	
	void port_init();
	watch_init();
	
	PORTA |= ((1 << PA1));
	_delay_ms(10);
	BIT_CLEAR(&PORTA, PA7);
	
	BIT_SET(&PORTA, PA6);
	
	PORTA &= ~((1 << PA1)); //reset
	
	
    while(1)		
    {			
		if (!(PIND & (1<<PD2)) && (PIND & (1<<PD3)))
		{
			_delay_ms(400);
			if (timer_flag == 1)
			{
				timer_flag = 0;
				timergo = 1;
			}
			else
			{
				timer_flag = 1;
			}
			
		}
		
		if (!(PIND & (1<<PD2)) && timer_flag == 1)
		{
			;
		}
		
		
		
		
		if (!(PIND & (1<<PD3)) && !(PIND & (1<<PD2)))
		{
			reset_flag = 0;
			_delay_ms(250);
			if (push_flag == 0)
			{
				if (set_clock_flag == 1)
				{
					set_clock_flag = 0;
				}
				else
				{
					set_clock_flag = 1;
				}
			}

			push_flag = 1;
		}
		else
		{
			push_flag = 0;
			if (set_clock_flag == 1 && !(PIND & (1<<PD3)) && (PIND & (1<<PD2)))
			{
				_delay_ms(150);
				main_time.minutes_l++;
				if (main_time.minutes_l > 9)
				{
					main_time.minutes_l = 0;
					main_time.minutes_h++;
					if (main_time.minutes_h > 5)
					{
						main_time.minutes_h = 0;
						main_time.hours_l++;
						
						if (((main_time.hours_l > 3) && (main_time.hours_h > 1)))
						{
							main_time.hours_h = 0;
							main_time.hours_l = 0;
						}
						if (main_time.hours_l > 9)
						{
							main_time.hours_l = 0;
							main_time.hours_h++;
						}
					}
				}
			}
			if (set_clock_flag == 1 && (PIND & (1<<PD3)) && !(PIND & (1<<PD2)))
			{
				_delay_ms(150);				
				main_time.hours_l++;
				if (((main_time.hours_l > 3) && (main_time.hours_h > 1)))
				{
					main_time.hours_h = 0;
					main_time.hours_l = 0;
				}
					if (main_time.hours_l > 9)
				{
					main_time.hours_l = 0;
					main_time.hours_h++;
				}				
			}
		}
		
    }
}

	
void watch_init()
{

		DDRA  = 0xff;			//DDRA set to output
		PORTA = 0x0;
		DDRB = 0xff;
		PORTB = 0x0;
		DDRC = 0xff;
		DDRD = 0xf3;			//PD2,PD3 input
		
		
		
		//Timer2 Start
		//ASSR = 0x08;			// asynchronous mode for timer2
		//BIT_SET(&ASSR, AS2);
		TCCR2 |= (1 << CS22) | (1 << CS21) | (1 << CS20);	//prescaler 1024
		TCNT2 = 0;
		TIMSK |= (1 << TOIE2);
		//Timer2 end
		
		
		TCCR0 |= (1 << CS01) | (1 << CS00) | (1 << COM00);			// prescaler 256;  COM00 toggle OC0 = PB3
		TCNT0 = 0;
		TIMSK |= (1 << TOIE0);			//interrupt enable 
		sei();				//global interrupt enable
}
//test

ISR(TIMER2_OVF_vect, ISR_NOBLOCK)
{
	static uint8_t tmr0_ctr = 0;
	tmr0_ctr ++;
	if (tmr0_ctr >= 61)
	{
		tmr0_ctr =0;
		main_time.seconds_l ++;
		if(main_time.seconds_l > 9)
		{
			main_time.seconds_l = 0;
			main_time.seconds_h++;
			if (main_time.seconds_h > 5)
			{
				main_time.seconds_h = 0;
				main_time.minutes_l++;
				if (main_time.minutes_l > 9)
				{
					main_time.minutes_l = 0;
					main_time.minutes_h++;
					if (main_time.minutes_h > 5)
					{
						main_time.minutes_h = 0;
						main_time.hours_l++;
						
						if (((main_time.hours_l > 3) && (main_time.hours_h > 1)))
						{
							main_time.hours_h = 0;
							main_time.hours_l = 0;
						}
						if (main_time.hours_l > 9)
						{
							main_time.hours_l = 0;
							main_time.hours_h++;
						}
					}
				}
			}
		}
	}	
}




 
ISR(TIMER0_OVF_vect, ISR_NOBLOCK)			// ISR kann unterbrochen werden
{

	static int pos_7seg = 0;			//position in 7segment
	static uint8_t output0 = SEG_X;
	static uint8_t output1 = SEG_0;
	static uint8_t output2 = SEG_0;
	static uint8_t output3 = SEG_X;
	
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
			if( !(output0 & (1<< pos_7seg)))
			{
				BIT_SET(&PORTA, PA3);
			}
			else
			{
				BIT_CLEAR(&PORTA, PA3);
			}
					
			if( !(output1 & (1<< pos_7seg)))
			{
				BIT_SET(&PORTA, PA1);
			}
			else
			{
				BIT_CLEAR(&PORTA, PA1);
			}
				
			if( !(output2 & (1<< pos_7seg)))
			{
				BIT_SET(&PORTC, PC0);
			}
			else
			{
				BIT_CLEAR(&PORTC, PC0);
			}
			
			if( !(output3 & (1<< pos_7seg)))
			{
				BIT_SET(&PORTD, PD7);
			}
			else
			{
				BIT_CLEAR(&PORTD, PD7);
			}	
		
		
			if (pos_7seg == 8 )
			{
				BIT_SET(&PORTA, PA2);		//write data to output
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
			output_set(main_time.minutes_l, &output3);
			output_set(main_time.minutes_h, &output2);
			output_set(main_time.hours_l, &output1);
			output_set(main_time.hours_h, &output0);	
		}		
	}	
}


void output_set(int number, uint8_t* output)
{
	if (reset_flag == 1 && (main_time.seconds_l % 2))
	{
		*output = SEG_NO;
	}
	else
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
}