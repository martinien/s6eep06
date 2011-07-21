#include "def.h"
#include <libpic30.h>

//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// => G�rer RSSI

//Tests:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=> Flash LED: 			OK
//=> Rotary encoder : 		OK
//=> Serial port (PC) : 	OK
//=> ADC 10bits : 			OK
//=> Serial port (GPS) : 	OK
//=> Serial port (LINX) :	OK
//=> RF Communication :		OK	
//=> GLCD :					OK

//Config fuses
//Refer to p24fj16ga004.h for the details
_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & ICS_PGx1 & FWDTEN_OFF);
_CONFIG2(IESO_OFF & SOSCSEL_LPSOSC & WUTSEL_FST & FNOSC_FRCPLL & FCKSM_CSDCMD & OSCIOFNC_ON);

//Warnings
#ifndef USE_GLCD
#warning "GLCD Disabled!"
#endif
#ifdef BORNE
#warning "Ce module est une BORNE!"
#endif
#ifdef AUTO
#warning "Ce module est une AUTO!"
#endif
#ifdef GPS_FEEDTHROUGH
#warning "GPS Feedthrough activated!"
#endif




//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                    Constants and variables                               //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//interrupt.c
extern volatile int nombre;	//Pour encodeur
extern volatile unsigned int adc_result[];
extern volatile unsigned rf_flag;

//Images bitmap converties
extern char test[];

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                        Main function                                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{	
	int last_nombre = 0;
	char str[] = "Test de String...";
	
	//Initial configuration
	setup_oscillator();
	config();
	
//	//D�mo num�risation:
//	while(1)
//	{
//		demo_numerisation();
//	}//D�mo... commenter au final

	//Test de communication radio:
//	radio_dir(TRM_RX);	//Module en r�ception
	radio_dir(TRM_TX);		//Module en �mission
	
	//Main loop
	while (1)
	{
		//Test: encodeur et GLCD
		if(last_nombre != nombre)	//Si une transition a eu lieu
		{
			switch(nombre)
			{
				case 0:
					GLCD_ClearScreen();
					GLCD_GoTo(0,0);
					GLCD_WriteString(str);
					GLCD_SetPixel(10, 20, 1);
					break;
				case 1:
					GLCD_Bitmap(test, 0, 0, 128, 64);
					break;
				default:
					GLCD_ClearScreen();
					GLCD_GoTo(0,0);
					GLCD_WriteChar(nombre + 48);
					break;
			}			
			last_nombre = nombre;
		}
		
		//Flags
		if(rf_flag)
		{
			rf_flag = 0;
			
			//On envoie ici les requ�tes:
			puts_usart2("Test de string!");	//� enlever
			//Commentaire: le display est lent car cette fonction est tr�s lente... 300bauds
		}
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
	
	//Assignation des p�riph�riques
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
	
	#ifdef USE_GLCD
	//Init GLCD
	GLCD_Initalize();
	//Delay10KTCYx(0);
	GLCD_ClearScreen();
	#endif		
}

//D�mo projet
void demo_numerisation(void)
{
	char str[10];
	
	sprintf(str, "ADC: %i\r", adc_result[1]);
	puts_usart1((char *)str);
	delay_us(10000);
}
