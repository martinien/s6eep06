#include "def.h"
#include <libpic30.h>

//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// => Gérer RSSI

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
extern volatile unsigned rfdelai_flag, rf_rx_flag, rf_delai_flag, envoie;
extern volatile char rx;
//Images bitmap converties
extern char Base1[], Base2[];

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                        Main function                                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{	//Test
	int last_nombre = 0;
	char str[] = "Test de String...";
	char tx = 0b00110101; //donnee de test de la couche application
	
	//Variables
	char trameRX[NBRFANION+2];
	char trame_complete = 0;
	unsigned decalage = 0, contexte_trame =0;		//Indique la position dans la réception de la trame	
	
	//Initial configuration
	setup_oscillator();
	config();
	
//	//Démo numérisation:
//	while(1)
//	{
//		demo_numerisation();
//	}//Démo... commenter au final

	//Test de communication radio:
	#ifdef BORNE
	radio_dir(TRM_RX);	//Module en réception
	#endif
	
	#ifdef AUTO
	radio_dir(TRM_TX);		//Module en émission
	#endif

	//Display welcome screen:	ToDo
	#ifdef USE_GLCD
	GLCD_Bitmap(Base1, 0, 0, 128, 64);
	#endif
	
	//Main loop
	while (1)
	{
		
		//Test: encodeur et GLCD
		if(last_nombre != nombre)	//Si une transition a eu lieu
		{
			switch(nombre)
			{
				case 0:
					#ifdef USE_GLCD
					GLCD_ClearScreen();
					GLCD_GoTo(0,0);
					GLCD_WriteString(str);
					GLCD_SetPixel(10, 20, 1);
					#endif
					break;
				case 1:
					#ifdef USE_GLCD
					GLCD_ClearScreen();
					GLCD_GoTo(0,0);
					GLCD_Bitmap(Base1, 0, 0, 128, 64);
					#endif
					break;
				case 2:
					#ifdef USE_GLCD
					GLCD_ClearScreen();
					GLCD_GoTo(0,0);
					GLCD_Bitmap(Base2, 0, 0, 128, 64);
					#endif
					break;
				default:
					#ifdef USE_GLCD
					GLCD_ClearScreen();
					GLCD_GoTo(0,0);
					GLCD_WriteChar(nombre + 48);
					#endif
					break;
			}			
			last_nombre = nombre;		
		}
		
//		//Donnée entrante
//		if(rf_rx_flag == 1 && trame_complete == 0)
//			rf_detection_trame(&rx, &decalage, &trame_complete, &trameRX, &contexte_trame);
		
		
		#ifdef BORNE
		//Protocole de liaison de données
		//Envoie de donnée
		/*if(envoie == 0 && tx!= 0b00000000)
		{
			envoie = rf_envoie(&tx);
		}*/
		////Attente de confirmation
		//if(envoie == 1)
		//{
		//	//Lecture de la confirmation
		//	if(rf_rx_flag == 1)
		//	{z
		//		//envoie = rf_valider_confirmation();
		//		rf_rx_flag == 0;
		//	}
		//	//Délai de réponse atteint
		//	if(rf_delai_flag == 1)
		//	{
		//		rf_delai_flag == 0;
		//		envoie = rf_envoie(&tx);
		//	}
		//}	
		#endif
		
		//Flags		
//		if(rf_flag)
//		{
//			rf_flag = 0;
//			
//			#ifdef AUTO
//			sprintf(str, "Value is : %i", nombre);
//			puts_usart2(str);
//			#endif
//			//Commentaire: le display est lent car cette fonction est très lente... 300bauds
//		}
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
	
	//Disable unwanted interrupts:
	_U1TXIE = 0;
	_U2TXIE = 0;
	_SPI1IE = 0;
	_SPI1IF = 0;
	_T3IE = 0;
	_T2IE = 0;
	_OC2IE = 0;
	_IC1IE = 0;
	_OC1IE = 0;
	_IC1IE = 0;
	_U1ERIE = 0;
	_U2ERIE = 0;
	_PMPIE = 0;
	
	#ifdef USE_GLCD
	//Init GLCD
	GLCD_Initalize();
	GLCD_ClearScreen();
	#endif		
}

//Démo projet
void demo_numerisation(void)
{
	char str[10];
	
	sprintf(str, "ADC: %i\r", adc_result[1]);
	puts_usart1((char *)str);
	delay_us(10000);
}
