#include "def.h"

volatile unsigned int refresh_led = 0;
#define REFRESH_RATE 9

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                         Interruptions                                    //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//Uart1 Receive
void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
	char rx = U1RXREG;	
	
	U1TXREG = rx;			//Echo
	IFS0bits.U1RXIF = 0;	//Clear flag
}

//Uart1 Emit
void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	//Should not happen...
	IFS0bits.U1TXIF = 0;	//Clear flag
}

//Timer 1 : 10ms
void __attribute__ ((interrupt, no_auto_psv)) _T1Interrupt(void)
{
	//LED
	if(refresh_led < REFRESH_RATE)
	{
		++refresh_led;
	}
	else
	{
		refresh_led = 0;
		ALIVE ^= 1;
	}
	
	_T1IF = 0;	//Clear flag
}


