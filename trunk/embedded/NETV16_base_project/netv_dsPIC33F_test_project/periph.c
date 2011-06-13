#include "periph.h"
#include "def.h"


//==========================================================================================//
// Function name: 		void define_io(void)												//
// Short description:	Define inputs and outputs											//
// In: 					Nop																	//
// Out:					Nop																	//
//==========================================================================================//

void define_io(void)
{
	//UART #1
	TRIS_RX = 1;
	TRIS_TX = 0;
	
	peripheral_pin_select();
}	

void peripheral_pin_select(void)
{	
	//UART2
	RPINR18bits.U1RXR = 24;		//UART1 RX => RP24	
	RPOR12bits.RP25R = 3;		//UART1 TX => RP25
}

//Timer 1, interrupt 10ms
void setup_timer1(void)
{
	T1CONbits.TON = 0;		//Timer1 Off
	T1CONbits.TSIDL = 0;	//Continue in idle
	T1CONbits.TGATE = 0;	//Gated time accumulation disabled
	T1CONbits.TCKPS = 1;	//Prescale 1:8 (1:1, 1:8, 1:64, 1:256)
	T1CONbits.TCS = 0;		//Internal clock (Fcy)
	
	//Period
	TMR1 = 0x00;
	PR1 = 0x4E20;			//10ms
	
	//Interrupts
	IPC0bits.T1IP = 1;		//Lowest priority
	IFS0bits.T1IF = 0;
	IEC0bits.T1IE = 1;
	
	T1CONbits.TON = 1;		//Timer1 On
}
