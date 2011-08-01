#include "def.h"
#include <libpic30.h>

//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// => G�rer RSSI

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
extern volatile int nombre;
extern volatile unsigned int adc_result[];
extern volatile unsigned int rf_flag;
extern volatile unsigned rf_delai_flag, rf_rx_flag, rf_delai_flag, envoie;
extern volatile char rx;
extern volatile int buttonPress;	//Pour encodeur
//Images bitmap converties
extern const unsigned char Base1 [];

extern unsigned int adc_rssi[];
extern unsigned int rssi_flag;

unsigned int rssi = 0;

char str[8];

//Borne
int borneChoisie = 0;
char ADRESSE1[] = "11, ";
char ADRESSE2[] = "26, StudioX";
char ADRESSE3[] = "99, Coin Viet";
char BATTSWITCH[] = "Batterie changee";
char ADR1A[] = "Adr1#  11, PotAuFeu";
char ADR2A[] = "Adr2#  26, StudioX";
char ADR3A[] = "Adr3#  99, rue Viet";
char ADR1[] = "Adr1   11, PotAuFeu";
char ADR2[] = "Adr2   26, StudioX";
char ADR3[] = "Adr3   99, Coin Viet";
char BATTERIE = 100;
char SERIALBATTERIE = 12;
unsigned int BORNERESERVE = 0;
int ecran = 1;
int toEcran1 = 0;
extern volatile unsigned char DISTANCE1;
extern volatile unsigned char DISTANCE2;
extern volatile unsigned char DISTANCE3;
char DISTANCE;
char DIST[] = "Distance";
char RESERVE[] = "Reserve";
char DISTANCEBATT = 0;	//distance minimum pour consid�rer la batterie chang�e

//Test:
char result = 0;
unsigned char index = 0;
unsigned char dirty_buf[16] = {0xF3, 0xF3, 0xF3, 0xF3, 0xF2, 0x52, 0x32, 0x20, 0x00,0x00};
unsigned char clean_buf[32] = {0,0,0,0,0,0,0,0,0,0};
unsigned char flag = 0x7E;

//fifo.c
extern unsigned char fifo[];

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                        Main function                                     //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{	//Test
	int last_nombre = 4;
	char str[64] = "Test de String...";
	unsigned char distanceActuel = 0;
	
	//Variables
	char trame_a_envoye = 0, trame_complete = 0, trameRX[NBROCTET], trameTX[NBROCTET];
	unsigned decalage = 0, contexte_trame =0;
	unsigned int contexte_local = 0, i =0;
	
	//Initial configuration
	setup_oscillator();
	config();
	
	//Init fifo
	fifo_init();
	radio_dir(TRM_RX);		//Module en r�ception

	//Display welcome screen:	ToDo
	#ifdef USE_GLCD
	GLCD_Bitmap((char*)Base1, 0, 0, 128, 64);
	#endif
	
	
	//Test
//	index = 0;
//	while(1)
//	{
//	//	result = get_offset(dirty_buf[index], dirty_buf[index + 1], flag);
//		result = get_flag(flag);
//		Nop();
//		clean_buffer(result, 16);
//		Nop();
//	}
	
	//Main loop
	while (1)
	{
		
		//Test: encodeur et GLCD
		if((last_nombre != nombre && ecran == 1) || toEcran1 == 1 || buttonPress)	//Si une transition a eu lieu
		{
			getBatt();
			toEcran1 = 0;
			GLCD_ClearScreen();
			GLCD_Bitmap((char*)Base1, 0, 0, 128, 64);
			if(buttonPress)
			{
				BORNERESERVE = nombre;
				GLCD_GoTo(30,5);
				GLCD_WriteString(RESERVE);
			}
			sprintf(str,"%d",SERIALBATTERIE);
			GLCD_GoTo(107,0);
			GLCD_WriteString(str);

			sprintf(str,"%d",BATTERIE);			
			GLCD_GoTo(80,0);
			GLCD_WriteString(str);

			switch(nombre)
			{
				case 0:
					#ifdef USE_GLCD
					DISTANCE = DISTANCE1;
					GLCD_GoTo(0,2);
					GLCD_WriteString(ADR1A);
				if(!buttonPress)
				{
					GLCD_GoTo(0,3);
					GLCD_WriteString(ADR2);
					GLCD_GoTo(0,4);
					GLCD_WriteString(ADR3);
				}
					#endif
					break;
				case 1:
					#ifdef USE_GLCD
					DISTANCE = DISTANCE2;
					GLCD_GoTo(0,3);
					GLCD_WriteString(ADR2A);
				if(!buttonPress)
				{
					GLCD_GoTo(0,2);
					GLCD_WriteString(ADR1);
					GLCD_GoTo(0,4);
					GLCD_WriteString(ADR3);
				}
					#endif
					break;
				case 2:
					#ifdef USE_GLCD
					DISTANCE = DISTANCE3;
				if(!buttonPress)
				{
					GLCD_GoTo(0,2);
					GLCD_WriteString(ADR1);
					GLCD_GoTo(0,3);
					GLCD_WriteString(ADR2);
				}
					GLCD_GoTo(0,4);
					GLCD_WriteString(ADR3A);
					#endif
					break;
				default:
					#ifdef USE_GLCD
					GLCD_ClearScreen();
					GLCD_Bitmap((char*)Base1, 0, 0, 128, 64);
					#endif
					break;
			}
			sprintf(str,"%d",DISTANCE);	
			GLCD_WriteString(str);
			distanceActuel = DISTANCE;	
			buttonPress = 0;		
			last_nombre = nombre;		
		}	
		
		//Donn�es sortantes
		if(trame_a_envoye == 1)
		{
			envoie = rf_envoie(trameTX, clean_buf);
			
			if(envoie == 1)
				trame_a_envoye = 0;
		}
		
		//Flags - Donn�es entrantes	
		if(rf_flag == 1)//&& trame_complete == 0)
		{
			
			result = get_flag(flag);
			if(result != 10)
			{
				clean_buffer(result, 32);

				//Extraction des octets vers la couche application
				trame_complete = rf_gerer_RX(clean_buf, trameRX);			
			}
			
			//Remise � z�ro de la d�tection
			rf_flag = 0;
			result = 10;

			
		}
		
		#ifdef GPS_FEEDTHROUGH
		//TestGPS
	
		float LA = 45.3793;
		float LoA= -71.9239;
		
		assignDist(LA,LoA);
		
		#endif

		#ifdef USE_GLCD
		/*
		if(buttonPress)
		{
			switchScreen(last_nombre);
			buttonPress = 0;
		}
		*/
		if(distanceActuel < DISTANCEBATT && BATTERIE < 50 && ecran ==2)
		{
			switchBatt();
		}
		#endif

		#ifdef BORNE
		if(buttonPress)
		{
			buttonPress = 0;
			char donnee_test[NBROCTET] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
			envoie = rf_envoie(&donnee_test, clean_buf);
		}
		#endif
		
		
		if(rssi_flag)
		{
			rssi_flag = 0;
			//Filtre les donn�es quand le buffer est plein
			rssi = get_rssi();
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


/*
void switchScreen(last_nombre)
{
	if(ecran == 1)
	{
		ecran = 2;
		borneChoisie = last_nombre;
		GLCD_ClearScreen();
		GLCD_Bitmap(Base1, 0, 0, 128, 64);

		sprintf(str,"%d",SERIALBATTERIE);
		GLCD_GoTo(107,0);
		GLCD_WriteString(SERIALBATTERIE);
		sprintf(str,"%d",BATTERIE);
		GLCD_GoTo(80,0);
		GLCD_WriteString(BATTERIE);	
		switch(borneChoisie)
		{
			case 0:
				GLCD_GoTo(0,2);
				GLCD_WriteString(ADR1);
				sprintf(str,"%d",DISTANCE1);
				GLCD_GoTo(0,7);
				GLCD_WriteString(str);
				break;
			case 1:
				GLCD_GoTo(0,3);
				GLCD_WriteString(ADR2);
				sprintf(str,"%d",DISTANCE2);
				GLCD_GoTo(38,7);
				GLCD_WriteString(str);
				break;
			case 2:
				GLCD_GoTo(0,4);
				GLCD_WriteString(ADR3);
				sprintf(str,"%d",DISTANCE3);
				GLCD_GoTo(38,7);
				GLCD_WriteString(str);
				break;
			default:
				break;
		}
	}
	else if(ecran == 2)
	{
		ecran = 1;
		GLCD_ClearScreen();
		GLCD_Bitmap(Base1, 0, 0, 128, 64);
		nombre = 0;
		toEcran1 = 1;
	}
}
*/
void switchBatt(void)
{
	GLCD_ClearScreen();
	GLCD_GoTo(0,3);
	GLCD_WriteString(BATTSWITCH);
	SERIALBATTERIE++;			//Faire une fonction allant chercher le nouveau serial
}

void getBatt(void)
{
	BATTERIE = (adc_result[1]>>3);
}

