/*
 * library.h
 *
 * Created: 25-Mar-19 5:58:14 PM
 *  Author: Anas Khedr
 */ 


#ifndef LIBRARY_H_
#define LIBRARY_H_

//mode argument
#define LOW_LEVEL 0
#define CHANGE	  1
#define FALLING	  2
#define RISING	  3

int8_t enable_interrupt(bool INTx=0, uint8_t mode=0b11);			//enable INT0 or INT1 and set the type of signal that will cause the interrupt from mode {LOW_LEVER, CHANGE, FALLING or RISING}
//bool interrupt_flag(bool INTx=0);
#define get_interrupt_flag(INTx) (EIFR & 1<<INTx)					//get the interrupt flag for INT0 or INT1
#define clear_interrupt_flag(INTx) (EIFR &=~ 1<<INTx)				//clear the interrupt flag manually if you want(if global and INTx interrupt is active then you won't be able to clear the flag before ISR() is called)
#define disable_interrupt(INTx) EIMSK &= ~(1<<INTx)					//disable INTx interrupt only (the global interrupt will remain the same unless cleared with cli() or set with sei() manually)
int8_t enable_pc_interrupt_pin(uint8_t PCINTx);						//enable PC interrupt for the specified pin PCINTx 
#define get_pc_interrupt_flag(PCIFx) PCIFR & 1<<PCIF##PCIFx			//get the interrupt flag just line in INTx
#define clear_pc_interrupt_flag(PCIFx) PCIFR &=~ 1<<PCIF##PCIFx		//clear the interrupt flag manually
void disable_pc_interrupt_pin(uint8_t PCINTx);						//disable PC interrupt if want to use the pin for another purpose
#define disable_pc_interrupts PCICR=0								//disable all the PC interrupt in the MCU 



#endif /* LIBRARY_H_ */