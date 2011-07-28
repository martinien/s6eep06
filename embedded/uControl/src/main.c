//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                     		[uControl!]                             		//
//                                  			Main                                  		//
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
#include <delays.h>
#include <stdlib.h>
#include <stdio.h>

//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// => 


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                    Definitions and variables                             //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

volatile unsigned int refresh = 0;
#define REFRESH_RATE 60
volatile char usart_buf_char = 0;

char uart_buf[128];


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                     	 	Main Function                                 	//
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void main(void)
{	
	unsigned char data0 = 0, data1 = 0, data2 = 0;
	unsigned char NumBorne = 5, NbBatt = 56, Queue = 8;
	unsigned char IdBattOut = 127, IdBattIn = 254;
	
	//Init
	pre_config();
	config();

	//RC6/RC6 as digital:
	ANSELCbits.ANSC7 = 0;
	ANSELCbits.ANSC6 = 0;

	//Peripherals
	setup_usart1();
	setup_timer1();

	//Interrupts:
	INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;  

	//Main loop
	while(1)
	{
		//Création des nombres:
		data0++;
		if(data0 > 99)
			data0 = 0;
			
		data1 = ((TMR1L >> 2)+(TMR1L >> 3));
		
		data2 = (data0 + data1);
		if(data2 > 99)
			data2 = data0;
//Old:			
//		//Émet un string sur le port série...
//		sprintf(uart_buf, "ID %02d NBBAT %02d QUEUE %02d", data0, data1, data2);
//	//	while(busy_usart1);
//		puts_usart1((char *)uart_buf);
		
		//Trame Séquence d’état de la borne
		sprintf(uart_buf, " ID %02d NBBAT %02d QUEUE %02d ", NumBorne, NbBatt, Queue);
		while(!busy_usart1);
		puts_usart1((char *)uart_buf);
		
		Delay10KTCYx(160);	//Attend 100ms

		//Trame Séquence check-in/check-out
		sprintf(uart_buf, " ID %02d OUT %04d IN %04d/ ", NumBorne, IdBattOut, IdBattIn);
		while(!busy_usart1);
		puts_usart1((char *)uart_buf);
		
		Delay10KTCYx(160);	//Attend 100ms				
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                           Functions                                      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//==========================================================================================//
// Function name: 		void pre_config(void)												//
// Short description:	µC config: PLL, etc.												//
// In: 					Nop																	//
// Out:					Nop																	//
//==========================================================================================//

void pre_config(void)
{
	//Oscillator
	
   //OSC Interne 16MHz (62.5ns/inst.)
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    //Wait for stabilization
    while(!OSCCONbits.HFIOFS);
}

//==========================================================================================//
// Function name: 		void config(void)													//
// Short description:	Init peripherals and pins											//
// In: 					Nop																	//
// Out:					Nop																	//
//==========================================================================================//

void config(void)
{
	//Input and output definitions
	define_io();		

}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                            Interruptions                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#pragma code highVector=0x08	// on déclare que lors d'une interruption
void atInterrupthigh(void)
{
_asm GOTO Interrupt_High _endasm       // on doit éxecuter le code de la fonction MyHighInterrupt
}
#pragma code // retour à la zone de code


#pragma interrupt Interrupt_High
void Interrupt_High(void)
{
	unsigned char sauv1;
    unsigned char sauv2;

    sauv1 = PRODL; // on sauvegarde le contenu des registres de calcul
    sauv2 = PRODH;       
	
	if(PIR1bits.TMR1IF) // interruption du timer1
	{
		TMR1H = 0x54;
		TMR1L = 0xDE;	
		PIR1bits.TMR1IF = 0;
		
		//LED
		if(refresh < REFRESH_RATE)
		{
			++refresh;
		}
		else
		{
			refresh = 0;
			ALIVE ^= 1;
		}
	}
	if(PIR1bits.RC1IF)	//USART1 RX
	{
		//Echo
		usart_buf_char = getc_usart1();
		putc_usart1(usart_buf_char);
	}
				
	PRODL = sauv1;        // on restaure les registres de calcul
    PRODH = sauv2;               
}
#pragma code

//Config
#pragma config FOSC = INTIO67, PLLCFG = ON, WDTEN = OFF, LVP = OFF, DEBUG = ON
