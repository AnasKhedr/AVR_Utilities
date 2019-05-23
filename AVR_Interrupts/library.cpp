/*
 * interrupt_lib.cpp
 *
 * Created: 25-Mar-19 5:45:50 PM
 * Author : Anas Khedr
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "library.h"


int8_t enable_interrupt(bool INTx, uint8_t mode){ // INTx is the interrupt the user wants to activate, mode is the signal that activates it(you could select the signal that activates it from the defined macros in the .h file)
	sei();
	if(INTx){//INT1 is requested to be enabled
		/*mode according to the datasheet
		The level and edges on the external INT1 pin that activate the interrupt are defined in
		the table below through ISC pins(Interrupt Sense Control) in EICRA(External Interrupt Control Register A)
		 ----------------------------------------------------------------------------
		|ISC11|	ISC10 |	Description													 |
		|  0  |   0	  |	The low level of INT1 generates an interrupt request.		 |
		|  0  |   1	  |	Any logical change on INT1 generates an interrupt request.	 |
		|  1  |   0	  |	The falling edge of INT1 generates an interrupt request.	 |
		|  1  |   1	  |	The rising edge of INT1 generates an interrupt request.		 |
		 ----------------------------------------------------------------------------
		*/
		EICRA |= mode<<ISC10;
		/*EIMSK(External Interrupt Mask Register) has two bits INT1 and INT0 that enables and disables the interrupt on INT0 and INT1
		 When the INT1 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the external pin interrupt
		 is enabled. The Interrupt Sense Control1 bits 1/0 (ISC11 and ISC10) in the External Interrupt Control Register A
		 (EICRA) define whether the external interrupt is activated on rising and/or falling edge of the INT1 pin or level
		 sensed. Activity on the pin will cause an interrupt request even if INT1 is configured as an output. The
		 corresponding interrupt of External Interrupt Request 1 is executed from the INT1 Interrupt Vector. 
		*/
		EIMSK |= 1<<INT1;
		return INT1_vect_num;//this return value should be passed to ISR(_VECTOR(return value)){ ...Your Code ....} ||NOTE that ISR() argument MUST to be passed at compile time
	}else{//INT0 is the one requested to be enabled
		/*according to the datasheet
		The level and edges on the external INT0 pin that activate the interrupt are defined in
		the table below through ISC pins(Interrupt Sense Control) in EICRA(External Interrupt Control Register A)
		 ----------------------------------------------------------------------------
		|ISC01|	ISC00 |	Description													 |
		|  0  |   0	  |	The low level of INT0 generates an interrupt request.		 |
		|  0  |   1	  |	Any logical change on INT0 generates an interrupt request.	 |
		|  1  |   0	  |	The falling edge of INT0 generates an interrupt request.	 |
		|  1  |   1	  |	The rising edge of INT0 generates an interrupt request.		 |
		 ----------------------------------------------------------------------------
		*/
		EICRA |= mode<<ISC00;
		/*EIMSK(External Interrupt Mask Register) has two bits INT1 and INT0 that enables and disables the interrupt on INT0 and INT1
		When the INT0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), the external pin interrupt
		is enabled. The Interrupt Sense Control0 bits 1/0 (ISC01 and ISC00) in the External Interrupt Control Register A
		(EICRA) define whether the external interrupt is activated on rising and/or falling edge of the INT0 pin or level
		sensed. Activity on the pin will cause an interrupt request even if INT0 is configured as an output. The
		corresponding interrupt of External Interrupt Request 0 is executed from the INT0 Interrupt Vector.
		*/
		EIMSK |= 1<<INT0;
		return INT0_vect_num;//this return value should be passed to ISR(_VECTOR(return value)){ ...Your Code ....}  ||NOTE that ISR() argument MUST to be passed at compile time
	}
	return -1;	//error, wrong interrupt number!
}
/*
#define get_interrupt_flag(INTx) (EIFR & 1<<INTx)
	When an edge or logic change on the INT0/1 pin triggers an interrupt request, INTF0/1 becomes set (one). If the Ibit
	in SREG and the INT0/1 bit in EIMSK are set (one), the MCU will jump to the corresponding Interrupt Vector.
	The flag is cleared when the interrupt routine is executed. Alternatively, the flag can be cleared by writing a
	logical one to it. This flag is always cleared when INT0/1 is configured as a level interrupt.
*/

int8_t enable_pc_interrupt_pin(uint8_t PCINTx){
	sei();
	//PCINTx is a number between 0 and 23 which will be replacing the x in PCINTx. 
	if(7>=PCINTx && PCINTx>=0){
		//PCINTx = PCINTx%8;//This line in useless because PCINTx is always less than 8
		/*PCMSK0 (Pin Change Mask 0) has 8 bits [PCINT7 PCINT6 PCINT5 PCINT4 PCINT3 PCINT2 PCINT1 PCINT0]
													7	  6      5      4      3      2      1       0
		 Each PCINT[7:0] bit selects whether pin change interrupt is enabled on the corresponding I/O pin. If PCINT[7:0]
		 is set and the PCIE0 bit in PCICR is set, pin change interrupt is enabled on the corresponding I/O pin. If
		 PCINT[7:0] is cleared, pin change interrupt on the corresponding I/O pin is disabled.   
		*/
		PCMSK0 |= 1<<PCINTx;
		/*PCICR (Pin Change Interrupt Control Register) has 3 bits PCIE0, PCIE1 and PCIE2 (Pin Change Interrupt Enable)
		When the PCIE0 bit is set (one) and the I-bit in the Status Register (SREG) is set (one), pin change interrupt 0 is
		enabled. Any change on any enabled PCINT[7:0] pin will cause an interrupt. The corresponding interrupt of Pin
		Change Interrupt Request is executed from the PCI0 Interrupt Vector. PCINT[7:0] pins are enabled individually
		by the PCMSK0 Register.
		*/
		//All PCINT[7:0] has the same ISR (Interrupt Service Routine) with the same flag bit (PCIF0 in PCIFR)
		PCICR  |= 1<<PCIE0;
		//use PCINT0_vect_num to tell the ISR that this is the interrupt you want
		//example: ISR(_VECTOR("the return of this function")){ ...Your Code ....}  ||NOTE that ISR() argument MUST to be passed at compile time
		return PCINT0_vect_num;
		
	}else if(14>=PCINTx && PCINTx>=8){//PCINT15 doesn't exist in Atmega328p
		PCINTx = PCINTx%8;
		/*PCMSK1 (Pin Change Mask 1) has 7 bits [  –  PCINT14 PCINT13 PCINT12 PCINT11 PCINT10 PCINT9 PCINT8]
												   7	  6      5      4        3       2      1       0
		bit 7 is an unused bit in the ATmega48A/PA/88A/PA/168A/PA/328/P, and will always read as zero.
		Each PCINT[14:8]-bit selects whether pin change interrupt is enabled on the corresponding I/O pin. If
		PCINT[14:8] is set and the PCIE1 bit in PCICR is set, pin change interrupt is enabled on the corresponding I/O
		pin. If PCINT[14:8] is cleared, pin change interrupt on the corresponding I/O pin is disabled.
		*/
		PCMSK1 |= 1<<PCINTx;
		//All PCINT[14:8] has the same ISR (Interrupt Service Routine) with the same flag bit (PCIF1 in PCIFR)
		PCICR  |= 1<<PCIE1;
		//use PCINT1_vect_num to tell the ISR that this is the interrupt you want
		//example: ISR(_VECTOR("the return of this function")){ ...Your Code ....}  ||NOTE that ISR() argument MUST to be passed at compile time
		return PCINT1_vect_num;
		
	}else if(23>=PCINTx && PCINTx>=16){
		PCINTx = PCINTx%8;
		/*PCMSK2 (Pin Change Mask 2) has 8 bits [PCINT23 PCINT22 PCINT21 PCINT20 PCINT19 PCINT18 PCINT17 PCINT16]
													7	    6      5        4       3       2       1       0
		 Each PCINT[23:16]-bit selects whether pin change interrupt is enabled on the corresponding I/O pin. If
		 PCINT[23:16] is set and the PCIE2 bit in PCICR is set, pin change interrupt is enabled on the corresponding I/O
		 pin. If PCINT[23:16] is cleared, pin change interrupt on the corresponding I/O pin is disabled.
		*/
		PCMSK2 |= 1<<PCINTx;
		//All PCINT[23:16] has the same ISR (Interrupt Service Routine) with the same flag bit (PCIF2 in PCIFR)
		PCICR  |= 1<<PCIE2;
		//use PCINT2_vect_num to tell the ISR that this is the interrupt you want
		//example: ISR(_VECTOR("the return of this function")){ ...Your Code ....}  ||NOTE that ISR() argument MUST to be passed at compile time
		return PCINT2_vect_num;
	}
	return -1;	//error, wrong interrupt number!
	//sei();	//you should choose if you want it here or not
}
void disable_pc_interrupt_pin(uint8_t PCINTx){//after i made this i can assume that nobody will ever use it cuz why?! 
	if(7>=PCINTx && PCINTx>=0){
		PCINTx = PCINTx%8;
		PCMSK0 &=~ 1<<PCINTx;
		
	}else if(14>=PCINTx && PCINTx>=8){//PCINT15 doesn't exist in Atmega328p
		PCINTx = PCINTx%8;
		PCMSK1 &=~ 1<<PCINTx;
		
	}else if(23>=PCINTx && PCINTx>=16){
		PCINTx = PCINTx%8;
		PCMSK2 &=~ 1<<PCINTx;
	}
}

