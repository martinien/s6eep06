#include "def.h"
#include "NETV16_SerialDriver.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                         Interruptions                                    //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//Uart1 Receive
void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
//	char rx = U1RXREG;	
	
	//Handle USART Interrupt
	serial_usart_interrupt_handler();
	
	//U1TXREG = rx;			//Echo
	IFS0bits.U1RXIF = 0;	//Clear flag
}

//Uart1 Emit
void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
	//Should not happen...
	IFS0bits.U1TXIF = 0;	//Clear flag
}


