/*
 * avrio.c
 *
 * Created: 16-Mar-19 10:45:14 PM
 * Author : Anas Khedr
 * Email  : anaskhedr47@gmail.com
 */ 

#ifndef __AK_AVRIO__
#define __AK_AVRIO__

#include <avr/io.h>

#define _DDR(letter) DDR##letter
#define _PORT(letter) PORT##letter
#define _PIN(letter) PIN##letter
#define _P(letter,num) P##letter##num

#define pinMode(port,pin,mode) (mode==0 ? (DDR##port &= ~(1<<pin)) : (DDR##port |= (1<<pin)))
#define digitalRead(port,pin) (PIN##port && (1<<pin))
#define digitalWrite(port,pin,_output_) (_output_ == 0 ? (PORT##port &= ~(1<<pin)) : (PORT##port |= (1<<pin)))
#define toggle(port,pin) (PORT##port ^= (1<<pin))
#define setPullUp(port,pin) (PORT##pport != (1<<pin))
#define resetPullUp(port,pin) (PORT##port &= !(1<<pin]))


#define INPUT 0
#define OUTPUT 1

typedef _Bool bool;

//void pinMode(char p,int8_t pin,bool mode);
//void digitalWrite(char p,int8_t pin,bool _output);  
//bool digitalRead(char p,int8_t pin);

void ADCSetup(uint8_t prescaler, _Bool mode);
uint16_t analoglRead(int8_t channel);
uint16_t toVolt(uint16_t val);
void PWMSetup(char OC0x);
void analoglWrite(char OC0x,float volt);

void inline disconnectOC0A(){
	TCCR0A &=~ 0b11<<COM0A0;		//Normal port operation, OC0A disconnected.
}

void inline disconnectOC0B(){
	TCCR0A &=~ 0b11<<COM0B0;		//Normal port operation, OC0B disconnected
}

#endif