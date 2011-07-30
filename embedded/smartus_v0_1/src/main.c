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
extern volatile int nombre;
extern volatile unsigned int adc_result[];
extern volatile unsigned int rf_flag;
extern volatile unsigned rfdelai_flag, rf_rx_flag, rf_delai_flag, envoie;
extern volatile char rx;
extern volatile int buttonPress;	//Pour encodeur
//Images bitmap converties
extern const unsigned char Base1 [];

extern unsigned int adc_rssi[];
extern unsigned int rssi_flag;

unsigned int rssi = 0;

//Borne
int borneChoisie = 0;
unsigned char ADRESSE1[] = "11, rue Galt, Sherb";
unsigned char ADRESSE2[] = "26, rue Well, Sherb";
unsigned char ADRESSE3[] = "99, rue Viet, Sherb";
unsigned char BATTSWITCH[] = "Batterie changée";
unsigned char ADR1[] = "Adresse 1";
unsigned char ADR2[] = "Adresse 2";
unsigned char ADR3[] = "Adresse 3";
unsigned char CHOIX[] = "#";
unsigned char batterie = 100;
int ecran = 1;
int toEcran1 = 0;
unsigned char DISTANCE1 = 0;
unsigned char DISTANCE2 = 0;
unsigned char DISTANCE3 = 0;
unsigned char DIST[] = "Distance";
unsigned char RESERVE[] = "Réservé";
unsigned char DISTANCEBATT = 10;		//distance minimum pour considérer la batterie changée

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
	int last_nombre = 0;
	char str[64] = "Test de String...";
	char tx = 0b00110101; //donnee de test de la couche application
	unsigned char distanceActuel = 0;
	
	//Variables
	char trame_complete = 0, trameRX[NBRFANION+2];
	unsigned decalage = 0, contexte_trame =0;
	unsigned int contexte_local = 0, i =0;
	
	//Initial configuration
	setup_oscillator();
	config();
	
	//Init fifo
	fifo_init();
	
//	//Démo numérisation:
//	while(1)
//	{
//		demo_numerisation();
//	}//Démo... commenter au final

	//Test de communication radio:
	#ifdef AUTO
	radio_dir(TRM_RX);		//Module en réception
	#endif
	
	#ifdef BORNE
	radio_dir(TRM_TX);		//Module en émission
	#endif

	//Display welcome screen:	ToDo
	#ifdef USE_GLCD
	GLCD_Bitmap(Base1, 0, 0, 128, 64);
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
		if((last_nombre != nombre && ecran == 1) || toEcran1 == 1)	//Si une transition a eu lieu
		{
			toEcran1 = 0;
			GLCD_ClearScreen();
			GLCD_Bitmap(Base1, 0, 0, 128, 64);
			
			GLCD_GoTo(0,2);
			GLCD_WriteString(ADR1);
			GLCD_GoTo(38,2);
			GLCD_WriteString(ADRESSE1);
			
			GLCD_GoTo(0,3);
			GLCD_WriteString(ADR2);
			GLCD_GoTo(38,3);
			GLCD_WriteString(ADRESSE2);
			
			GLCD_GoTo(0,4);
			GLCD_WriteString(ADR3);
			GLCD_GoTo(38,4);
			GLCD_WriteString(ADRESSE3);
			
			GLCD_GoTo(38,5);
			GLCD_WriteString(DIST);			
			switch(nombre)
			{
				case 0:
					#ifdef USE_GLCD
					GLCD_GoTo(30,2);
					GLCD_WriteString(CHOIX);
					GLCD_GoTo(38,7);
					GLCD_WriteString(DISTANCE1);
					distanceActual = DISTANCE1;
					#endif
					break;
				case 1:
					#ifdef USE_GLCD
					GLCD_GoTo(30,3);
					GLCD_WriteString(CHOIX);
					GLCD_GoTo(38,7);
					GLCD_WriteString(DISTANCE2);
					distanceActual = DISTANCE2;
					#endif
					break;
				case 2:
					#ifdef USE_GLCD
					GLCD_GoTo(30,4);
					GLCD_WriteString(CHOIX);
					GLCD_GoTo(38,7);
					GLCD_WriteString(DISTANCE3);
					distanceActual = DISTANCE3;
					#endif
					break;
				default:
					#ifdef USE_GLCD
					GLCD_ClearScreen();
					GLCD_Bitmap(Base1, 0, 0, 128, 64);
					#endif
					break;
			}			
			last_nombre = nombre;		
		}	
		
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
		
		//Flags - Données entrantes	
		if(rf_flag)//&& trame_complete == 0)
		{
//			rf_flag = 0;
//			
//			rf_detection_trame(&rx, &decalage, &trame_complete, &trameRX, &contexte_trame);
//			
//			//Test de réception synchronisé		//pascal
//			if(trame_complete == 1)
//			{
//				rf_rx_flag = 0;
//				trame_complete = 0;
//
//			}
			
			result = get_flag(flag);
			if(result != 10)
			{
				clean_buffer(result, 32);
				Nop();				
			}

			#ifdef BORNE

			#endif

		}

		#ifdef BORNE
		//Pascal...
		#endif
		
		#ifdef AUTO
		
		if(buttonPress)
		{
			switchScreen(last_nombre);
			buttonPress = 0;
		}
		
		if(distanceActuel < DISTANCEBATT)
		{
			switchBatt();
		}
		
		if(rssi_flag)
		{
			rssi_flag = 0;
			//Filtre les données quand le buffer est plein
			rssi = get_rssi();
		}
		
		#endif	
		
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

void send_trame(void)
{
	char trame[20];
	unsigned int i = 0;
	
	for(i = 0; i < 10; i++)
	{
		trame[i] = 'a';
	}
	for(i = 0; i < 10; i++)
	{
		trame[10+i] = '0'+i;
	}
	
	for(i = 0; i < 20; i++)
	{
		while(busy_usart2());
		U2TXREG = trame[i];
	}
}

//Retourne de combien de caractères on doit shifter à gauche
//Une valeur de 10 indique "aucun match"

//Bug connu: fausse détection si 0x00 suivi de 0xFF. Sera corrigé sous peu! - JFD

char get_offset(unsigned char msb, unsigned char lsb, unsigned char ref)
{
	int i = 0;
	unsigned int value_16 = (msb << 8) + lsb;
	unsigned int mask = (ref << 8);
	#ifdef DEBUG_MPSIM
	unsigned int temp1 = 0, temp2 = 0;
	#endif
	
	if((msb == 0xFF) || (lsb == 0xFF))
		return 10;	//Impossible que ce soit bon
	
	if((value_16 & 0xFF00) == mask)
		return 0;	//Pas d'offset
	
	for(i = 1; i < 8; ++i)
	{
		mask = mask >> 1;
		#ifdef DEBUG_MPSIM
		temp1 = (value_16 & mask);			
		temp2 = (unsigned int)(ref << (8 - i));
		if(temp1 == temp2)
		{
//			if((i == 7) && (value_16 == 0xFF))
//				return 10;
//			else
				return i;
		}

		#else
		if((value_16 & mask) == (unsigned int)(ref << (8 - i))
			return i;
		#endif
	}	
	
	//Aucun match!
	return 10;
}

//Assume 2 buffers fixes, celui de réception (fifo[]) et celui nettoyé
//Décale les données et rempli le buffer clean
void clean_buffer(unsigned char offset, char buf_length)
{
	unsigned int temp1 = 0;
	int i = 0;
	
	for(i = 0; i < buf_length-1; i++)
	//for(i = buf_length-1; i > 0; i--)
	{
		//ToDo: Optimiser
		temp1 = (fifo[i] << 8) + fifo[i+1];
		temp1 = temp1 << offset;
		temp1 = (temp1 & 0xFF00) >> 8;		
		
		clean_buf[i] = temp1;
	}
}

unsigned int get_rssi(void)
{
	unsigned int i = 0;
	unsigned long sum = 0;
	
	for(i = 0; i < AVG; i++)
	{
		sum += adc_rssi[i];
	}
	
	return (sum >> 4);
}

unsigned char get_flag(unsigned char flag)
{
	int i = 0;
	char off;
	
	for(i = 0; i < (FIFO_LENGTH-2); i++)
	{
		off = get_offset(fifo[i], fifo[i+1], flag);
		if(off != 10)
			break;
	}
	
	return off;
}

void switchScreen(last_nombre)
{
	if(ecran == 1)
	{
		ecran = 2;
		borneChoisie = last_nombre;
		GLCD_ClearScreen();
		GLCD_Bitmap(Base1, 0, 0, 128, 64);
		GLCD_GoTo(0,5);
		GLCD_WriteString(RESERVE);
		switch(borneChoisie)
		{
			case 0:
				GLCD_GoTo(0,2);
				GLCD_WriteString(ADR1);
				GLCD_GoTo(38,2);
				GLCD_WriteString(ADRESSE1);
				break;
			case 1:
				GLCD_GoTo(0,3);
				GLCD_WriteString(ADR2);
				GLCD_GoTo(38,3);
				GLCD_WriteString(ADRESSE2);
				break;
			case 2:
				GLCD_GoTo(0,4);
				GLCD_WriteString(ADR3);
				GLCD_GoTo(38,4);
				GLCD_WriteString(ADRESSE3);
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

void switchBatt(void)
{
	GLCD_ClearScreen();
	GLCD_GoTo(38,3);
	GLCD_WriteString(BATTSWITCH);
}
