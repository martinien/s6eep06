#include "def.h"

volatile unsigned int refresh_led = 0;
volatile int nombre = 0;	//Pour encodeur
volatile int buttonPress = 0;	//Pour encodeur
volatile int timer1 = 0;
volatile unsigned int adc_channel = 0;
volatile unsigned int adc_result[2] = {0,0};
volatile unsigned rf_cnt = 0, rf_flag = 0, rf_rx_flag = 0, envoie =0, rf_delai_flag =0;
volatile char rx = '0';
#define REFRESH_RATE 9

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                         Interruptions                                    //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//Uart1 Receive: USB/GPS
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

//Uart2 Receive: RF
void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void)
{
	char dump = '0';
	
	if(U2STAbits.FERR)
	{	
		//Si erreur de framing, on dump
		dump = U2RXREG;
	}
	else
	{
		//Sinon, on utilise la valeur
		rx = U2RXREG;
		rf_rx_flag = 1;
		
		//On effectue une action uniquement si RSSI est assez haut	
		if(1)	//ToDo: RSSI
		{	
			U1TXREG = rx;				//Echo sur UART1 (USB)
			
			#ifdef BORNE
			if((rx >= '0') && (rx <= '5'))
			{
				#ifdef USE_GLCD
				GLCD_GoTo(0,0);
				GLCD_WriteChar(rx);		//Affiche sur le GLCD
				#endif
			}
			#endif
		}
	}
	
	IFS1bits.U2RXIF = 0;	//Clear flag
}

//Uart2 Emit
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void)
{
	//Should not happen...
	IFS1bits.U2TXIF = 0;	//Clear flag
}

//Timer 1 : 10ms
void __attribute__ ((interrupt, no_auto_psv)) _T1Interrupt(void)
{
	timer1++;
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
	
	//RF
	if(envoie)
	{
		++rf_cnt;	
	}
	if(rf_cnt >= 39)	// ~400ms
	{
		rf_cnt = 0;
		rf_delai_flag = 1;
	}	
	
	//Start and ADC conversion
	if(adc_channel >= 1)
		adc_channel = 0;
	else
		++adc_channel;
		
	AD1CHSbits.CH0SA = adc_channel;
	AD1CON1bits.SAMP = 1;	//New conversion

	
	_T1IF = 0;	//Clear flag
}

//INT0 : Encoder switch (press)
void __attribute__ ((interrupt, no_auto_psv)) _INT0Interrupt(void)
{
	buttonPress = 1;
	GLCD_ClearScreen();
	
	_INT0IF = 0;	//Clear flag
}

//INT1 : Encoder A (rotation)
void __attribute__ ((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
    if(ENC_SWB == 0)	//Rotation horaire
		++nombre;
    else				//Rotation anti-horaire
		--nombre;
		
	if(nombre > 2)
		nombre = 0;
	if(nombre < 0)
		nombre = 2;	
	
	_INT1IF = 0;	//Clear flag
}

//ADC1
void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
	adc_result[adc_channel] = ADC1BUF0;
	
	_AD1IF = 0;	//Clear flag
}
