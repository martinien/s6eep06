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
	//LED
	TRIS_ALIVE = 0;
	
	//Analogique
	TRIS_RSSI = 1;
	TRIS_POT = 1;
	
	//Encodeur
	TRIS_ENC_SW = 1;
	TRIS_ENC_SWA = 1;
	TRIS_ENC_SWB = 1;
	
	//UART PC (UART-USB)
	TRIS_RXUSB = 0;
	TRIS_TXUSB = 1;
	
	//UART GPS
	TRIS_RXGPS = 0;
	TRIS_TXGPS = 1;
	TRIS_PPS = 1;
	
	//UART Radio (Combiné RX et TX)
	TRIS_TXRX = 0;
	TR = 0;
	TRIS_TR = 0;
}	

void peripheral_pin_select(void)
{
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
