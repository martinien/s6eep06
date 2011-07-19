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
	//INT1 - SWA - RB4/RP4
	RPINR0bits.INT1R = 4;	
	
	//UART1 : 
	#ifdef GPS_FEEDTHROUGH
	//Transfert les messages GPS au PC
	RPOR7bits.RP15R = 3;		//PIC TX = RB15/RP15 : USB	
	RPINR18bits.U1RXR = 3;		//PIC RX = RB3/RP3 : GPS
	#else
	//GPS ou PC (commenter au choix)
	RPINR18bits.U1RXR = 14;		//PIC RX = RB14/RP14 : USB
	RPOR7bits.RP15R = 3;		//PIC TX = RB15/RP15 : USB	
//	RPINR18bits.U1RXR = 3;		//PIC RX = RB3/RP3 : GPS
//	RPOR1bits.RP2R  = 3;		//PIC TX = RB2/RP2 : GPS
	#endif

}

void radio_dir(unsigned char dir)
{
	if(dir == TRM_TX)
	{
		//Transmission
		TRIS_TXRX = 0;			//Output
		RPOR4bits.RP9R = 5;		//RP9 = U2TX
		TR = 1;					//Transmit mode
	}
	else
	{
		//Réception
		TRIS_TXRX = 1;			//Input
		RPOR4bits.RP9R = 0;		//No peripheral output
		RPINR19bits.U2RXR = 9;	//RP9 = U2RX
		TR = 0;					//Receive mode
	}
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

void setup_adc(void)
{
	AD1CON1bits.ADON = 0;		//ADC Off
	AD1CON1bits.ADSIDL = 0;		//Continue in idle
	AD1CON1bits.FORM = 0;		//Integer (unsigned)
	AD1CON1bits.SSRC = 7;		//Auto-convert
	AD1CON1bits.ASAM = 0;		//No auto sampling
	
	AD1CON2bits.VCFG = 0;		//Ref = AVdd & AVss
	AD1CON2bits.CSCNA = 0;		//Use the channel selected by CHOSA
	AD1CON2bits.SMPI = 0;		//Interrupt after each conversion
	AD1CON2bits.BUFM = 0;		//One 16-word buffer
	AD1CON2bits.ALTS = 0;		//Always use MUXA
	
	AD1CON3bits.ADRC = 1;		//Internal ADC clock
	AD1CON3bits.SAMC = 31;		//Auto-sample time
	
	AD1CHSbits.CH0NA = 0;		//Negative input is Vr-
	AD1CHSbits.CH0SA = 0;		//Sample AN0	
		
	AD1PCFG = 0xFFFC;			//All Digital except AN0 & AN1
	
	//Enable ints and module:
	_AD1IF = 0;
	_AD1IE = 1;
	AD1CON1bits.ADON = 1;		//ADC On
	AD1CON1bits.SAMP = 1;		//New conversion
}

void delay_us(unsigned int t)
{
	unsigned int repeat = 0;
	
	for(repeat = 0; repeat < t; repeat++)	//Environ 1.68µs
	{
		Nop();	
 	}  	
}
