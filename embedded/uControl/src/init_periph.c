//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                     		[uControl!]                             		//
//                                       Init Peripherals                                	//
//                                       	version 1.0                                     //
//																		                    //
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                       		  PIC18F44k22, 8MHz xPLL = 10MIPS         					//
//                                         	 JFDuval                                        //
//                                        	30/01/2011         		                        //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#include "def.h"
#include "init_periph.h"

//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// => 


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                    Definitions and variables                             //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                           Functions                                      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//==========================================================================================//
// Function name: 		void define_io(void)												//
// Short description:	Set inputs and outputs												//
// In: 					Nop																	//
// Out:					Nop																	//
//==========================================================================================//

void define_io(void)
{
	//Analog (user)
	TRIS_AN0 = 1;
	TRIS_AN1 = 1;
	TRIS_AN2 = 1;
	TRIS_AN3 = 1;
	TRIS_AN4 = 1;
	TRIS_AN5 = 1;
	TRIS_AN6 = 1;
	TRIS_AN7 = 1;

	//Digital (user)
	DIO_0 = 0;
	TRIS_DIO_0 = 0;
	DIO_1 = 0;
	TRIS_DIO_1 = 0;
	DIO_2 = 0;
	TRIS_DIO_2 = 0;
	DIO_3 = 0;
	TRIS_DIO_3 = 0;
	DIO_4 = 0;
	TRIS_DIO_4 = 0;
	DIO_5 = 0;
	TRIS_DIO_5 = 0;
	DIO_6 = 0;
	TRIS_DIO_6 = 0;
	DIO_7 = 0;
	TRIS_DIO_7 = 0;

	//Power outputs (user)
	PWR_0 = 0;
	TRIS_PWR_0 = 0;
	DIO_1 = 0;
	TRIS_PWR_1 = 0;
	PWR_2 = 0;
	TRIS_PWR_2 = 0;
	PWR_3 = 0;
	TRIS_PWR_3 = 0;
	PWR_4 = 0;
	TRIS_PWR_4 = 0;
	PWR_5 = 0;
	TRIS_PWR_5 = 0;

	//Analog (pre-defined)
	TRIS_TEMP = 1;
	TRIS_AMP = 1;
	TRIS_VOLT1 = 1;
	TRIS_VOLT2 = 1;
	
	//Misc.
	ALIVE = 0;
	TRIS_ALIVE = 0;
	TRIS_RX = 1;
	TRIS_TX = 1;	
}

//Setup Timer1
void setup_timer1(void)
{
	TMR1L = 0x00;
	
	T1CONbits.TMR1CS = 1;	//Clk = Fosc
	T1CONbits.T1CKPS = 4;	//PS = 8
	T1CONbits.T1RD16 = 0;	//8bits
	T1CONbits.TMR1ON = 1;
	
	PIR1bits.TMR1IF = 0;
	PIE1bits.TMR1IE = 1;
}