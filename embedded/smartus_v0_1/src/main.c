#include "def.h"

//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// =>

//Config fuses
//Refer to p24fj16ga004.h for the details
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & ICS_PGx1 & FWDTEN_OFF);
_CONFIG2(IESO_OFF & SOSCSEL_LPSOSC & WUTSEL_FST & FNOSC_FRCPLL & FCKSM_CSDCMD & OSCIOFNC_ON);


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                    Constants and variables                               //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//interrupt.c
extern volatile int nombre;	//Pour encodeur

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                        Main function                                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{	
	//Initial configuration
	setup_oscillator();
	config();

	//Main loop
	while (1)
	{

	}

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                           Fonctions                                      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void setup_oscillator(void)
{
	//Internal 8MHz oscillator with 4xPLL
	//16MIPS (62.5ns/inst.)
	OSCCONbits.COSC = 1;	//8MHz FRC w/PLL
	OSCCONbits.NOSC = 1;	//8MHz FRC w/PLL
	
	CLKDIVbits.DOZE = 0;	//CPU clock = peripheral clock (1:1)
	CLKDIVbits.RCDIV = 0;	//No division
	
	// the CPU will automatically switch when all is stable....
	while(!OSCCONbits.LOCK);  // Wait for PLL to lock
}

void config(void)
{	
	//Define inputs/outputs
	define_io();		
	
	//Assignation des périphériques
	peripheral_pin_select();
	
	//Config peripherals:
	setup_usart1(); 		//PC/GPS
	setup_usart2(); 		//Radio
	setup_timer1();			//10ms timebase
	setup_adc();			//ADC
	
	//Encoder interrupts:
	_INT0IF = 0;
	_INT1IF = 0;
	_INT0IE = 1;
	_INT1IE = 1;
}
