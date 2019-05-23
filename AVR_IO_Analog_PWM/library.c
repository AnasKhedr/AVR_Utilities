/*
 * avrio.c
 *
 * Created: 16-Mar-19 10:45:14 PM
 * Author : Anas Khedr
 * Email  : anaskhedr47@gmail.com
 */ 

#include "library.h"
#include <avr/io.h>
#include "avr/interrupt.h"

bool mode_buf;				//this buf is to store the mode chossend when analogSetup() first called. it's require by analogRead() to know which mode the ADC is working in 

void ADCSetup(uint8_t prescaler, _Bool mode){
	mode_buf = mode;
	/*
	 *The channel is the pin that will be connected to the ADC (Note that you have only one ADC with an input multiplexer that connects different pins)
	 *The successive approximation circuitry requires an input clock frequency should be between 50kHz and 200kHz to get maximum resolution
	 These bits determine the division factor between the system clock frequency and the input clock to the ADC
	 ---------------------------------
	|ADPS2|ADPS1|ADPS0|Division Factor|
	|  0  |  0  |  0  |		2         |
	|  0  |  0  |  1  |		2         |
	|  0  |  1  |  0  |		4         |
	|  0  |  1  |  1  |		8         |
	|  1  |  0  |  0  |		16        |
	|  1  |  0  |  1  |		32        |
	|  1  |  1  |  0  |		64        |
	|  1  |  1  |  1  |		128       |
	 ---------------------------------
	* the mode sets 8-bit(mode=1) or 10-bit (mode=0)
	*/
	
	
	ADCSRA |= _BV(ADEN);		//Enable the ADC 
	
	if(mode){
		ADMUX |= _BV(ADLAR);	//to easily read only 8 bits in 8 bit mode
	}else{
		ADMUX &= ~_BV(ADLAR);	//to easily read the complete 10 bits in 10 bit mode
	}							//note that the ADC always works in 10 bit mode, you only select what to read
	
	switch (prescaler)			//select the ADC clock
	{							// prescaler = F_CPU/(50KHZ to 200KHZ)
		case(2):
			ADCSRA |= 1;
			break;
		case(4)://2^2=4
			ADCSRA |= 2;
			break;
		case(8)://2^3=8
			ADCSRA |= 3;
			break;
		case(16):
			ADCSRA |= 4;
			break;
		case(32):
			ADCSRA |= 5;
			break;
		case(64):
			ADCSRA |= 6;
			break;
		case(128):
			ADCSRA |= 7;
			break;
		default:
			ADCSRA |= 3;
			break;
	}
	
}

uint16_t analoglRead(int8_t channel){
	
	DDRC &= ~_BV(channel);
	if(channel>6){				//there are only 6 channels
		//return -1;
		}else{
		ADMUX &= ~0x0f;			//reset the MUX bit first
		ADMUX |= channel;		//select the channel desired
	}
	
	ADCSRA |= _BV(ADSC);			//signal ADC to start converting 
	
	while(ADCSRA & _BV(ADSC));		//wait till ADC finishes reading
	
	if(mode_buf){					//8-bit mode
		return ADCH;
	}
	else{							//10-bit mode
		return ADCL |(ADCH<<8);		//ADCL will be read first due to Short Circuit Evaluation
	}
	
}

uint16_t toVolt(uint16_t val){	//assuming the ref voltage is 5V
	if(mode_buf){	//8-bit mode
		return (val*5/255);					//2^8  = 255
	}else{		//10-bit mode
		return (val*5/1023);				//2^10 = 1023
	}
}

void PWMSetup(char OC0x){			//this mush be called before using analogWrite()
	TCCR0A |= 0b11<<WGM00;			//set fast PWM mode
	TCCR0B |= 1<<CS00;				//set the Timer\Counter clk to the same as the MCU
	
	if(OC0x == 'A'){
		DDRD |= 1<<6;				//set 0C0A as output (PD6 for atmega328p)
		TCCR0A |= 1<<COM0A1;		//Clear OC0A on Compare Match, set OC0A at BOTTOM
	}
	else if(OC0x == 'B'){
		DDRD |= 1<<5;				//set 0C0B as output (PD5 for atmega328p)
		TCCR0A |= 1<<COM0B1;		//Clear OC0B on Compare Match, set OC0B at BOTTOM
	}
	else{							//activate both channels
		DDRD |= 1<<6;				//set 0C0A as output (PD6 for atmega328p)
		DDRD |= 1<<5;				//set 0C0B as output (PD5 for atmega328p)
		TCCR0A |= 1<<COM0A1;		//Clear OC0A on Compare Match, set OC0A at BOTTOM
		TCCR0A |= 1<<COM0B1;		//Clear OC0B on Compare Match, set OC0B at BOTTOM
	}
}

void analoglWrite(char OC0x,float volt){//initPWM() API mush be called before using analogWrite()
	
	uint8_t count_val = volt*255/5;
	
	if(OC0x == 'A'){
		//DDRD |= 1<<6;				//set 0C0A as output (PD6 for atmega328p)
		//TCCR0A |= 1<<COM0A1;		//Clear OC0A on Compare Match, set OC0A at BOTTOM
		OCR0A = count_val;			//configure the voltage you want to output
	}
	else if(OC0x == 'B'){
		//DDRD |= 1<<5;				//set 0C0B as output (PD5 for atmega328p)
		//TCCR0A |= 1<<COM0B1;		//Clear OC0B on Compare Match, set OC0B at BOTTOM
		OCR0B = count_val;			//configure the voltage you want to output
	}
	else{
		//error
	}
}

/*
void disconnectOC0A(){
	TCCR0A &=~ 0b11<<COM0A0;		//Normal port operation, OC0A disconnected.
}

void disconnectOC0B(){
	TCCR0A &=~ 0b11<<COM0B0;		//Normal port operation, OC0B disconnected
}*/