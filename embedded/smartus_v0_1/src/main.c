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
extern volatile unsigned rf_delai_flag, rf_rx_flag, rf_delai_flag, envoie;
extern volatile char rx;
extern volatile int buttonPress;	//Pour encodeur
//Images bitmap converties
extern const unsigned char Base1 [];

extern unsigned int adc_rssi[];
extern unsigned int rssi_flag;
extern unsigned int gps_flag;

unsigned int rssi = 0;

char str[8];

//Borne
char BATTSWITCH[] = "Batterie changee";
char ADR1A[] = "Adr1#  11, Pot Au Feu";
char ADR2A[] = "Adr2#  26, Studio Sex";
char ADR3A[] = "Adr3#  99, Coin Viet ";
char ADR1[] = "Adr1   11, Pot Au Feu";
char ADR2[] = "Adr2   26, Studio Sex";
char ADR3[] = "Adr3   99, Coin Viet ";
char BATTERIE = 100;
char SERIALBATTERIE = 12;
unsigned int BORNERESERVE = 3;
int ecran = 1;
extern volatile int toEcran1;
int DISTANCE;
char DIST[] = "Distance";
char RESERVE[] = "Reserve";
char DISTANCEBATT = 0;	//distance minimum pour considérer la batterie changée
char METER[] = "metres";
char DISTANCEPROCHE = 0; //Mettre normalement à 100
int confirm=2;

#ifdef BORNE
//Donnée de la borne initiale
unsigned char NumBorne = 2, NbBatt = 5, IdBattOut, IdBattIn, batterieId[5] = {1, 2, 7, 12, 16};
unsigned int  EtatBorne = 0, Queue = 0, batterie_a_reprendre =0;
#endif
	
#ifdef AUTO
unsigned int  EtatAuto = 0, BORNECONFIRME = 3, batterieId = 6;

#endif

//GPS
extern char DISTANCE1[];
extern char DISTANCE2[];
extern char DISTANCE3[];
extern char gpsstr[];
char gps[50];
extern float LaA;
extern float LoA;
int a;
extern int D1,D2,D3;

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
	
	//Variables protocole de liaison de donnée
	char trame_a_envoye = 0, trame_complete = 0, trameRX[NBROCTET], trameTX[NBROCTET];
	
	//Initial configuration
	setup_oscillator();
	config();
	
	//Init fifo
	fifo_init();
	radio_dir(TRM_RX);		//Module en réception

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

		#ifdef AUTO
		routine_auto(&trame_a_envoye, &trame_complete, trameTX, trameRX);
		#endif

		#ifdef BORNE
		routine_borne(&trame_a_envoye, &trame_complete, trameTX, trameRX);
		#endif
	
		
		//Test: encodeur et GLCD
		if((last_nombre != nombre) || buttonPress)	//Si une transition a eu lieu
		{
			#ifdef USE_GLCD
			
			GLCD_ClearScreen();
			GLCD_Bitmap((char*)Base1, 0, 0, 128, 64);
			if(buttonPress && !toEcran1)
			{
				BORNERESERVE = nombre;
				GLCD_GoTo(0,5);
				GLCD_WriteString(RESERVE);
				toEcran1 = 1;
			}
			else if(buttonPress && toEcran1)
			{
				toEcran1 = 0;
				BORNERESERVE = 3;
				confirm=2;
			}
			sprintf(str,"%d",SERIALBATTERIE);
			GLCD_GoTo(107,0);
			GLCD_WriteString(str);

			#endif
				switch(nombre)
				{
					case 0:
						#ifdef USE_GLCD
						DISTANCE = D1;
						GLCD_GoTo(0,2);
						GLCD_WriteString(ADR1A);
					if(!toEcran1)
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
						DISTANCE = D2;
						GLCD_GoTo(0,3);
						GLCD_WriteString(ADR2A);
					if(!toEcran1)
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
						DISTANCE = D3;
					if(!toEcran1)
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
			#ifdef USE_GLCD
			
			GLCD_GoTo(90,7);
			GLCD_WriteString(METER);
			distanceActuel = DISTANCE;	
			buttonPress = 0;		
			last_nombre = nombre;
			if(confirm==2)
			{
				if(D1<DISTANCEPROCHE){
					borneProche(0);	
				}
				else if(D2<DISTANCEPROCHE){
					borneProche(1);
				}
				else if(D3<DISTANCEPROCHE){
					borneProche(2);
				}
				if(confirm==1)
				{
					buttonPress=1;
					toEcran1=0;
				}
				else if(confirm==0)
				{
					buttonPress=0;
					toEcran1=0;
					last_nombre=4;	
				}
			}
			#endif
		}	
		
		//Données sortantes
		if(trame_a_envoye == 1)
		{
			envoie = rf_envoie(trameTX, (char *)clean_buf);
			
			if(envoie == 1)
				trame_a_envoye = 0;
		}
		
		//Flags - Données entrantes	
		if(rf_flag == 1)//&& trame_complete == 0)
		{
			//Actualise la batterie
			#ifdef USE_GLCD
			getBatt();
			sprintf(str,"%03i",BATTERIE);			
			GLCD_GoTo(80,0);
			if(nombre==0)
			{DISTANCE = D1;}
			else if(nombre==1)
			{DISTANCE = D2;}
			else if(nombre==2)
			{DISTANCE = D3;}
			
			GLCD_WriteString(str);
			sprintf(str,"%i",DISTANCE);
			GLCD_GoTo(40,7);
			GLCD_WriteString(str);
			#endif

			result = get_flag(flag);
			if(result != 10)
			{
				clean_buffer(result, 32);

				//Extraction des octets vers la couche application
				trame_complete = rf_gerer_RX((char *)clean_buf, trameRX);			
			}
			
			//Remise à zéro de la détection
			rf_flag = 0;
			result = 10;
		}
		
		//
		
		#ifdef GPS_FEEDTHROUGH
		//TestGPS
		#ifdef DEBUG_MPSIM
			gps_flag =1;  //REMOVE
		#endif
		
		if(gps_flag)
		{
			gps_flag = 0;
			//Filtre les données quand le buffer est plein
			
			#ifdef DEBUG_MPSIM
				(gpsstr[3]='R'); //REMOVE
			#endif
			
			if((gpsstr[3]=='R'))
			{
				for(a =0;a<47;a++)
				{
					gps[a] = gpsstr[a]; //Buffer de travail
				}
			//	puts_usart1(gpsstr);
				//gps[] = "$GPRMC,174254.000,V,4522.75800,N,07155.43400,W";
				#ifdef DEBUG_MPSIM
					gps[20]='4';
					gps[21]='5';
					gps[22]='2';
					gps[23]='2';
					gps[25]='7';
					gps[26]='5';
					gps[27]='8';
					
					gps[33]='0';
					gps[34]='7';
					gps[35]='1';
					gps[36]='5';
					gps[37]='5';
					gps[39]='4';
					gps[40]='3';
					gps[41]='4';
					gps[45]='W';
				#endif
				convStr();
				
				//LaA = 45.3793;
				//LoA= -71.9239;
				
				assignDist(LaA,LoA);				
			}
		}
		#endif
/*
		#ifdef USE_GLCD

		if(buttonPress)
		{
			switchScreen(last_nombre);
			buttonPress = 0;
		}
		
		if(distanceActuel < DISTANCEBATT && BATTERIE < 50 && ecran ==2)
		{
			switchBatt();
		}
		#endif
*/
		
		if(buttonPress)
		{
			#ifdef BORNE
			buttonPress = 0;
			char donnee_test[NBROCTET] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
			envoie = rf_envoie(donnee_test, (char *)clean_buf);
			#endif
			#ifdef AUTO
			puts_usart1(gps);
			#endif
		}
		
		
		
		if(rssi_flag)
		{
			rssi_flag = 0;
			//Filtre les données quand le buffer est plein
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

void switchBatt(void)
{
	GLCD_ClearScreen();
	GLCD_GoTo(0,3);
	GLCD_WriteString(BATTSWITCH);
	SERIALBATTERIE++;			//Faire une fonction allant chercher le nouveau serial
	BORNERESERVE = 4;
	confirm=2;
}
*/
void getBatt(void)
{
	BATTERIE = (adc_result[1]>>3);
}

#ifdef AUTO
void routine_auto(char *flag_TX, char *flag_RX, char *data_a_envoie, char *data_recu)
{
	unsigned int temp;
	
	//Si utilisateur a fait une demande de réservation et que la borne doit être informé
	if(BORNERESERVE != 3 && BORNECONFIRME == 3 && EtatAuto == 0 && *flag_TX == 0)
	{
		//Envoie de la demande de réservation
		sprintf(data_a_envoie, "B%02iEEEEE", BORNERESERVE);
		*flag_TX = 1;
		EtatAuto = 1;
	}
	
	//Si la borne confirme
	if(EtatAuto == 1 && *flag_TX == 0)
	{
		BORNECONFIRME = BORNERESERVE;
		EtatAuto = 2;
	}
	
	//Si l'utilisateur retire la réservation
	if(BORNERESERVE == 3 && EtatAuto == 2 && *flag_TX == 0)
	{
		//Envoie de la demande d'annulation
		sprintf(data_a_envoie, "C%02iEEEEE", BORNECONFIRME);
		*flag_TX = 1;
		EtatAuto = 3;
	}
	
	//Réception de la confirmation de la réservation annulé
	if(EtatAuto == 3 && *flag_TX == 0)
	{
		BORNECONFIRME = 0;
		EtatAuto = 0;
	}
	
	//Si on fait l'échange de batterie
//	if(BORNERESERVE == 4 && *flag_TX == 0)
//	{
//		//Signifie à la borne le changement de batterie
//		sprintf(data_a_envoie, "D%02i%02iEEEE", BORNECONFIRME, batterieId);
//		*flag_TX = 1;
//		EtatAuto = 4;
//	}
//	
//	//Confirmation du changement, attente de l'id de la nouvelle batterie
//	if(EtatAuto == 4 && *flag_TX == 0)
//	{
//		EtatAuto = 5;
//	}
//	
//	//Réception du nouvelle Id de la batterie, retour au mode par défault
//	if(EtatAuto == 5 && *flag_RX == 1)
//	{		
//		if(data_recu[4] != "0")
//		{
//			temp = atoi(data_recu[1]);
//			temp = temp + atoi(data_recu[5]);
//			batterieId = temp;
//			
//		}
//		else
//		{
//			batterieId = atoi(data_recu[5]);
//		}
//		
//		EtatAuto = 0;
//		*flag_RX = 0;
//	}
	
}
#endif

#ifdef BORNE
void routine_borne(char *flag_TX, char *flag_RX, char *data_a_envoie, char *data_recu)
{
	unsigned int temp;
	//Réception de donnée 
	if(*flag_RX == 1)
	{
		//Demande de réservation
		if(data_recu[0] == 'B' && EtatBorne == 0)
		{
			char uart_buf[];
			sprintf(uart_buf, " ID %02d NBBAT %02d QUEUE %02d ", NumBorne, NbBatt, Queue);
			while(!busy_usart1);
			puts_usart1((char *)uart_buf);

			Queue = Queue+1;
			*flag_RX = 0;
		}
		//Demande d'annulation de réservation
		if(data_recu[0] == 'C' && EtatBorne == 0)
		{
			
			char uart_buf[];
			sprintf(uart_buf, " ID %02d NBBAT %02d QUEUE %02d ", NumBorne, NbBatt, Queue);
			while(!busy_usart1);
			puts_usart1((char *)uart_buf);
			
			//Protection de retournement
			if(Queue > 0)
			{
				Queue = Queue-1;
			}
			*flag_RX = 0;
		}
//		//L'auto signifie le changement de batterie
//		if(data_recu[0] == 'D' && EtatBorne == 0)
//		{
//			//Sauvegarde du numéro de la batterie remis à la borne dans une variable temporaire
//			if(data_recu[1] != '0')
//			{
//				temp = atoi(data_recu[1]);
//				temp = temp + atoi(data_recu[2]);
//				batterie_a_reprendre = temp;
//				
//			}
//			else
//			{
//				batterie_a_reprendre = atoi(data_recu[1]);
//			}
//			*flag_RX = 0;
//			EtatBorne = 1;
//		}
	}
	
//	//Envoie de l'id de la nouvelle batterie de l'auto
//	if(EtatBorne == 1 && *flag_TX ==0)
//	{
//		
//		sprintf(*data_a_envoie, "D%02dEEEEE", batterieId[0]);
//		*flag_TX = 1;
//		EtatBorne = 2;
//	}
//	
//	//Attente de confirmation de l'auto à propos de l'id de la nouvelle batterie
//	if(EtatBorne == 2 && *flag_TX ==0)
//	{
//		//Faire le switch de batterie et l'envoyer à marcoux*** todo
//		//Sauvegarde de la batterie nouvelle stocker
//		temp = batterieId[0];
//		batterieId[0] = batterie_a_reprendre;
//		EtatBorne = 0;
//	}
}
#endif

#ifdef USE_GLCD
void borneProche(int sug)
{
	int nombrep=3;
	int alt = 0;
	confirm=0;
	GLCD_ClearScreen();
	GLCD_GoTo(0,5);
	GLCD_WriteString("Reserver?");
	switch(sug)
	{
		case 0:
			GLCD_GoTo(0,3);
			GLCD_WriteString(ADR1);
		break;
		case 1:
			GLCD_GoTo(0,3);
			GLCD_WriteString(ADR2);
		break;
		case 2:
			GLCD_GoTo(0,3);
			GLCD_WriteString(ADR3);
		break;
	}
	while(!buttonPress)
	{
		if(nombre!=nombrep && alt)
		{
			alt = 0;
			nombrep=nombre;
			confirm=1;
			GLCD_GoTo(0,6);
			GLCD_WriteString("oui");
		}
		else if(nombre!=nombrep && !alt)
		{
			alt = 1;
			nombrep=nombre;
			confirm=0;
			GLCD_GoTo(0,6);
			GLCD_WriteString("non");
		}
	}
	if(confirm==1)
		nombre=sug;
}
#endif
