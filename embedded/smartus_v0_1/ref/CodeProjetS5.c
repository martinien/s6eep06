//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                   [Projet S5P06 - UdeS]                                  //
//                                   Fonctionnement Global                                  //
//                                       version 1.0                                        //
//																		                    //
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                              PIC18F4550, 4MHz x PLL = 12MIPS                             //
//                                         JFDuval                                          //
//                                        30/10/2010         		                        //
//                      Louis-Philippe Brault et Philippe Labonté                           //
//                                        14/12/2010                                        //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#include <p18f4550.h>
#include <delays.h>
#include <timers.h>
#include <portb.h>
#include <usart.h>
#include "init_periph.h"
#include "KS0108.h"
#include "graphic.h"
#include "def.h"
//#include "config.asm"
//#include "read_euart.asm"
//#include "write_euart.asm"

//ToDo:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// => Convertir plusieurs fonctions en ASM
// => Configurer l'EEPROM pour y stocker des images de menus (In process)


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                    Constantes et variables                               //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

volatile int nombre = 0;		//Pour encodeur
volatile int emplacement = 1;	//Pour bouger dans menu
volatile int play = 0;			//Pour activer ou desactiver
volatile int pause = 1;
volatile int etatplay = 0;
volatile int proj = 1;
volatile int strob = 1;
volatile int vu = 1;
volatile unsigned char modeselect = 0;
volatile int bit3 = 1;
unsigned char status = 0;
unsigned char fft1 = 0;
unsigned char fft2 = 0;
unsigned char fft3 = 0;
unsigned char fft4 = 0;
unsigned char fft5 = 0;
unsigned char fft6 = 0;
unsigned char fft7 = 0;
unsigned char fft8 = 0;
unsigned char fft9 = 0;
unsigned char fft10 = 0;
unsigned char fft11 = 0;
unsigned char fft12 = 0;
unsigned char fft13 = 0;
unsigned char fft14 = 0;
unsigned char fft15 = 0;
unsigned char fft16 = 0;

int compteur_fft = 1;
volatile unsigned char read1= 1;
volatile unsigned char read2= 1;
volatile unsigned char read3= 1;
volatile unsigned char read4= 1;
volatile unsigned char read5= 1;
unsigned char i2c_actuateur = 0;
unsigned char data = 0;
volatile int nb_read=0;
volatile int data_ready=0;
int debut = 1;			//Depart
int toune = 0;			//# de la chanson
unsigned char euart_out = 0;
volatile unsigned char euart_in = 0;
volatile int flag_euart = 0;
volatile int flag_i2c = 0;
volatile int temp = 0;
int depart_euart = 0;
unsigned char music_1[] = "Rock ballad (7)   ";
unsigned char music_2[] = "Shuff leBlues (11)";
unsigned char music_3[] = "MedPops Ptn (28)  ";
unsigned char tst_str[4] = {0xFF, 0x44, 0x21, '0'};
unsigned char last_i2c_1 = 0, last_i2c_2 = 0;

//Prototypes
void Interrupt_High(void);
void config_port_b(void);
void barrgraph(short, short,short,short,short);	//Phil
short value2graph(short);					//Phil
void graphFFT(short,short,short,short,short,short,short,short,short,short,short,short,short,short,short,short);	//Phil
void graphVU(short,short);
unsigned char envoyer_i2c_noaddr(unsigned char);
//Prototypes des fonctions asm
extern void config_euart(void);
extern void read_euart(void);
extern void write_euart(void);

//Images bitmap converties
extern const rom char logo[];
extern const rom char ICONES[];
extern const rom char MODEselec[];
extern const rom char SPOTselec[];
extern const rom char STROBselec[];
extern const rom char BARRselec[];
extern const rom char flecheICO[];
extern const rom char flecheNULL[];
extern const rom char titreMODE[];
extern const rom char PLAY[];
extern const rom char PAUSE[];

int last_nombre = 1;
short chan_L = 16;
short chan_R = 24;

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                         Interruptions                                    //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


//*========================================================================================*\\
// Function name: 		void Interrupt_High(void)											\\
// Short description:																		\\
// In: 					Nop																	\\
// Out:					Nop																	\\
//*========================================================================================*\\


//Interuptions haut niveau
#pragma code highVector=0x08	// on déclare que lors d'une interruption
void atInterrupthigh(void)
{
_asm GOTO Interrupt_High _endasm       // on doit éxecuter le code de la fonction MyHighInterrupt
}
#pragma code // retour à la zone de code

//#pragma interrupt Interrupt_High
#pragma interrupt Interrupt_High
void Interrupt_High(void)
{
	unsigned char sauv1;
    unsigned char sauv2;
    
    sauv1 = PRODL; // on sauvegarde le contenu des registres de calcul
    sauv2 = PRODH;       

	//Lecture de l'octet recu par EUART et classement selon l'ordre de réception
	if(PIR1bits.RCIF)
	{
		euart_in = RCREG;

		if(euart_in ==0b11111111) //Ajout pour syncroniser le bit de start avec read 1
			{				
				nb_read=1;
			}
			else if(euart_in==0xFE)
				nb_read = 5;
			if(nb_read==1)
			{
				read1=euart_in;
				nb_read++;
			}
			else if(nb_read==2)
			{
				read2=euart_in;
				nb_read++;
			}
			else if(nb_read==3)
			{
				read3=0x00;
				read3=euart_in;
				nb_read++;
			}
			else if(nb_read==4)
			{
				read4=euart_in;
				nb_read++;
			}
			else if(nb_read>=5)
			{
				read5=euart_in;
				nb_read=0;
				flag_i2c=1;
			}

		data_ready=1;
		PIR1bits.RCIF=0;
	}

    //Gestion du bouton encodeur pour le changement des graphiques
    if(INTCON3bits.INT2IF)	//INT2 = Encodeur SWA
    {	
		if(emplacement==1)		//Dans le menu principal
		{
		    if(ENC_SWB == 0)	//Rotation horaire
				++nombre;
		    else				//Rotation anti-horaire
				--nombre;
				
			if(nombre > 3)
				nombre = 3;
			if(nombre < 0)
				nombre = 0;
				
		    INTCON3bits.INT2IF = 0;	
		}
		else if(emplacement==2)	//Dans le menu mode
		{
		    if(ENC_SWB == 0)	//Rotation horaire
				++nombre;
		    else				//Rotation anti-horaire
				--nombre;
				
			if(nombre > 8)
				nombre = 8;
			if(nombre < 4)
				nombre = 4;
				
		    INTCON3bits.INT2IF = 0;
		}    
	}
	

    PRODL = sauv1;        // on restaure les registres de calcul
    PRODH = sauv2;                          
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                           Fonctions                                      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

//*========================================================================================*\\
// Function name: 		void initialisation(void)											\\
// Short description:	Initialisation du fonctionnement (périphs, I/O, etc)				\\
// In: 					Nop																	\\
// Out:					Nop																	\\
//*========================================================================================*\\

void initialisation(void)
{
	//ADC Off
	ADCON0bits.ADON = 0;
	ADCON1 = 0b00001111;
	
	//Définition des IO
	define_i_o();
	
	//Périphériques		
//	init_timer_0();
	config_port_b();
	init_i2c();	
	init_eusart();
	//config_euart();
	
	//Init GLCD
	GLCD_Initalize();
	Delay10KTCYx(0);		//Modifier?
	GLCD_ClearScreen();		
	
	// On autorise toutes les interruptions
	RCONbits.IPEN = 0;	//1 niveau
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                      Fonction Principale                                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void main()
{
	unsigned char i2cdata = 0, usartdata = 0;
	unsigned char data = 0;
	unsigned char str[] = "nom de la toune";
	unsigned char AUTO[] = "Auto";
	unsigned char NORMAL[] = "Normal";
	unsigned char CUTOM1[] = "Custom 1";
	unsigned char CUTOM2[] = "Custom 2";
	unsigned char CUTOM3[] = "Custom 3";
	// Config position
	short xposMMODE = 40;
	short yposMMODE = 2; //(tranche de 8)


	initialisation();	//Initialise tout


LED1=0;
LED2=1;
LED3=1;
		while(debut)
		{
			//chech dsp pour signal autocorrélation puis debut à 1
			if (read1==0xFF)
				debut=0;
			LED1 = 0;
		}
		LED1 = 1;	
		//Boucle infinie
	while(1)
	{
		//Affichage de l'écran initial
		if(!debut)
		{
			GLCD_Bitmap(logo, 0, 0, 128, 64);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			GLCD_ClearScreen();
			GLCD_Bitmap(PAUSE, 47, 2, 35, 24);
			debut=1;
		}

		if(last_nombre != nombre)	//Si une transition a eu lieu, gérer les menus d'affichage
		{
			switch(nombre)
			{
				case 0:
					
					GLCD_Bitmap(ICONES, 0, 6, 128, 16);
					GLCD_Bitmap(MODEselec, 1, 6, 68, 16);
					GLCD_GoTo(5,7);
					if(modeselect == 0)
						GLCD_WriteString(AUTO,1);
					else if(modeselect == 1)
						GLCD_WriteString(NORMAL,1);	
					else if(modeselect == 2)
						GLCD_WriteString(CUTOM1,1);							
					else if(modeselect == 3)
						GLCD_WriteString(CUTOM2,1);
					else if(modeselect == 4)
						GLCD_WriteString(CUTOM3,1);							
					if(proj)
						GLCD_Bitmap(flecheICO, 75, 5, 8, 8); //SPOT
					else
						GLCD_Bitmap(flecheNULL, 75, 5, 8, 8); //SPOT
					if(strob)
						GLCD_Bitmap(flecheICO, 95, 5, 8, 8); //STROB
					else
						GLCD_Bitmap(flecheNULL, 95, 5, 8, 8); //STROB	
					if(vu)
						GLCD_Bitmap(flecheICO, 115, 5, 8, 8); //BARR
					else
						GLCD_Bitmap(flecheNULL, 115, 5, 8, 8); //BARR
					break;
				case 1:
					//GLCD_ClearScreen();
					GLCD_Bitmap(ICONES, 0, 6, 128, 16);
					GLCD_Bitmap(SPOTselec, 70, 6, 18, 16);
					GLCD_GoTo(5,7);
					if(modeselect == 0)
						GLCD_WriteString(AUTO,0);
					else if(modeselect == 1)
						GLCD_WriteString(NORMAL,0);	
					else if(modeselect == 2)
						GLCD_WriteString(CUTOM1,0);							
					else if(modeselect == 3)
						GLCD_WriteString(CUTOM2,0);
					else if(modeselect == 4)
						GLCD_WriteString(CUTOM3,0);							
					if(proj)
						GLCD_Bitmap(flecheICO, 75, 5, 8, 8); //SPOT
					else
						GLCD_Bitmap(flecheNULL, 75, 5, 8, 8); //SPOT
					if(strob)
						GLCD_Bitmap(flecheICO, 95, 5, 8, 8); //STROB
					else
						GLCD_Bitmap(flecheNULL, 95, 5, 8, 8); //STROB	
					if(vu)
						GLCD_Bitmap(flecheICO, 115, 5, 8, 8); //BARR
					else
						GLCD_Bitmap(flecheNULL, 115, 5, 8, 8); //BARR
					break;
				case 2:
					//GLCD_ClearScreen();
					GLCD_Bitmap(ICONES, 0, 6, 128, 16);
					GLCD_Bitmap(STROBselec, 90, 6, 18, 16);
					GLCD_GoTo(5,7);
					if(modeselect == 0)
						GLCD_WriteString(AUTO,0);
					else if(modeselect == 1)
						GLCD_WriteString(NORMAL,0);	
					else if(modeselect == 2)
						GLCD_WriteString(CUTOM1,0);							
					else if(modeselect == 3)
						GLCD_WriteString(CUTOM2,0);
					else if(modeselect == 4)
						GLCD_WriteString(CUTOM3,0);							
					if(proj)
						GLCD_Bitmap(flecheICO, 75, 5, 8, 8); //SPOT
					else
						GLCD_Bitmap(flecheNULL, 75, 5, 8, 8); //SPOT
					if(strob)
						GLCD_Bitmap(flecheICO, 95, 5, 8, 8); //STROB
					else
						GLCD_Bitmap(flecheNULL, 95, 5, 8, 8); //STROB	
					if(vu)
						GLCD_Bitmap(flecheICO, 115, 5, 8, 8); //BARR
					else
						GLCD_Bitmap(flecheNULL, 115, 5, 8, 8); //BARR
					break;
				case 3:
					//GLCD_ClearScreen();
					GLCD_Bitmap(ICONES, 0, 6, 128, 16);
					GLCD_Bitmap(BARRselec, 110, 6, 18, 16);
					GLCD_GoTo(5,7);
					if(modeselect == 0)
						GLCD_WriteString(AUTO,0);
					else if(modeselect == 1)
						GLCD_WriteString(NORMAL,0);	
					else if(modeselect == 2)
						GLCD_WriteString(CUTOM1,0);							
					else if(modeselect == 3)
						GLCD_WriteString(CUTOM2,0);
					else if(modeselect == 4)
						GLCD_WriteString(CUTOM3,0);								
					if(proj)
						GLCD_Bitmap(flecheICO, 75, 5, 8, 8); //SPOT
					else
						GLCD_Bitmap(flecheNULL, 75, 5, 8, 8); //SPOT
					if(strob)
						GLCD_Bitmap(flecheICO, 95, 5, 8, 8); //STROB
					else
						GLCD_Bitmap(flecheNULL, 95, 5, 8, 8); //STROB	
					if(vu)
						GLCD_Bitmap(flecheICO, 115, 5, 8, 8); //BARR
					else
						GLCD_Bitmap(flecheNULL, 115, 5, 8, 8); //BARR
					break;
				case 4:
					GLCD_ClearScreen();
					GLCD_Bitmap(titreMODE, 0, 0, 128, 16);
					GLCD_GoTo(xposMMODE,yposMMODE);
					GLCD_WriteString(AUTO,1);
					GLCD_GoTo(xposMMODE,yposMMODE+1);
					GLCD_WriteString(NORMAL,0);
					GLCD_GoTo(xposMMODE,yposMMODE+2);
					GLCD_WriteString(CUTOM1,0);
					GLCD_GoTo(xposMMODE,yposMMODE+3);
					GLCD_WriteString(CUTOM2,0);
					GLCD_GoTo(xposMMODE,yposMMODE+4);
					GLCD_WriteString(CUTOM3,0);
					break;	
				case 5:
					GLCD_ClearScreen();
					GLCD_Bitmap(titreMODE, 0, 0, 128, 16);
					GLCD_GoTo(xposMMODE,yposMMODE);
					GLCD_WriteString(AUTO,0);
					GLCD_GoTo(xposMMODE,yposMMODE+1);
					GLCD_WriteString(NORMAL,1);
					GLCD_GoTo(xposMMODE,yposMMODE+2);
					GLCD_WriteString(CUTOM1,0);
					GLCD_GoTo(xposMMODE,yposMMODE+3);
					GLCD_WriteString(CUTOM2,0);
					GLCD_GoTo(xposMMODE,yposMMODE+4);
					GLCD_WriteString(CUTOM3,0);
					break;
				case 6:
					GLCD_ClearScreen();
					GLCD_Bitmap(titreMODE, 0, 0, 128, 16);
					GLCD_GoTo(xposMMODE,yposMMODE);
					GLCD_WriteString(AUTO,0);
					GLCD_GoTo(xposMMODE,yposMMODE+1);
					GLCD_WriteString(NORMAL,0);
					GLCD_GoTo(xposMMODE,yposMMODE+2);
					GLCD_WriteString(CUTOM1,1);
					GLCD_GoTo(xposMMODE,yposMMODE+3);
					GLCD_WriteString(CUTOM2,0);
					GLCD_GoTo(xposMMODE,yposMMODE+4);
					GLCD_WriteString(CUTOM3,0);
					break;
				case 7:
					GLCD_ClearScreen();
					GLCD_Bitmap(titreMODE, 0, 0, 128, 16);
					GLCD_GoTo(xposMMODE,yposMMODE);
					GLCD_WriteString(AUTO,0);
					GLCD_GoTo(xposMMODE,yposMMODE+1);
					GLCD_WriteString(NORMAL,0);
					GLCD_GoTo(xposMMODE,yposMMODE+2);
					GLCD_WriteString(CUTOM1,0);
					GLCD_GoTo(xposMMODE,yposMMODE+3);
					GLCD_WriteString(CUTOM2,1);
					GLCD_GoTo(xposMMODE,yposMMODE+4);
					GLCD_WriteString(CUTOM3,0);
					break;
				case 8:
					GLCD_ClearScreen();
					GLCD_Bitmap(titreMODE, 0, 0, 128, 16);
					GLCD_GoTo(xposMMODE,yposMMODE);
					GLCD_WriteString(AUTO,0);
					GLCD_GoTo(xposMMODE,yposMMODE+1);
					GLCD_WriteString(NORMAL,0);
					GLCD_GoTo(xposMMODE,yposMMODE+2);
					GLCD_WriteString(CUTOM1,0);
					GLCD_GoTo(xposMMODE,yposMMODE+3);
					GLCD_WriteString(CUTOM2,0);
					GLCD_GoTo(xposMMODE,yposMMODE+4);
					GLCD_WriteString(CUTOM3,1);
					break;					
			}
			last_nombre = nombre;
			}
			//Si on appuit sur le bouton, activer l'actuateur en question ou changer de menu
			if(!ENC_SW)
			{
				while(!ENC_SW);
				Delay10KTCYx(125);
				switch(nombre)
					{
						case 0:
							emplacement=2;
							nombre=modeselect+4;
							last_nombre=0;
						break;
						case 1:
						if(proj)
							proj=0;
						else
							proj=1;
						last_nombre=0;
						break;
						case 2:
						if(strob)
							strob=0;
						else
							strob=1;
						last_nombre=0;
						break;
						case 3:
						if(vu)
							vu=0;
						else
							vu=1;
						last_nombre=0;
						break;
						case 4:
							modeselect=0;
							emplacement=1;
							nombre=0;
							GLCD_ClearScreen();
						break;
						case 5:
							modeselect=1;
							emplacement=1;
							nombre=0;
							GLCD_ClearScreen();
						break;
						case 6:
							modeselect=2;
							emplacement=1;
							nombre=0;
							GLCD_ClearScreen();
						break;
						case 7:
							modeselect=3;
							emplacement=1;
							nombre=0;
							GLCD_ClearScreen();
						break;
						case 8:
							modeselect=4;
							emplacement=1;
							nombre=0;
							GLCD_ClearScreen();
						break;
					}	
			}
		
		//Si la pédale est appuyée, changer de on-off
		if(!PORTBbits.RB3)
		{
			while(!PORTBbits.RB3);		//Attente que la pédale soit relâché
			if(etatplay==0)
			{
				play=1;
				pause=0;
				etatplay=1;
			}
			else
			{
				pause=1;
				last_nombre=9;
				etatplay=0;
			}
		}
// ---------------------------------------------------------------------------------------
// PLAY
		if(play)
		{
			//Affiche play à l'écran
			GLCD_Bitmap(PLAY, 47, 2, 35, 24);
			
			//Envoie le mode vers le dsp
			euart_out=modeselect;
			while(BusyUSART());
			putcUSART(euart_out);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			Delay10KTCYx(0);
			GLCD_ClearScreen();
			last_nombre=9;	
			play=1;
		}
// ---------------------------------------------------------------------------------------
// PAUSE
		if(emplacement == 1)
		{
			//Si non-pause, afficher les barres graphiques
			if(pause==0)
			{
				graphVU(chan_L,chan_R);
			}
			//Si pause, afficher l'image du pause
			else if(pause==1 && last_nombre!=nombre)
			{
				GLCD_ClearScreen();			
				GLCD_Bitmap(PAUSE, 47, 2, 35, 24);
				last_nombre=9;
			}
			//Affiche le nom de la chanson en cours
			GLCD_GoTo(2,0);
			if(toune==1)
				GLCD_WriteString(music_1,0);						
			else if(toune==2)
				GLCD_WriteString(music_2,0);
			else if(toune==3)
				GLCD_WriteString(music_3,0);			
		}

// I2C,  actualiser la FFT et envoyer les données vers le contrôleur principal
		if(flag_i2c)
		{
			flag_i2c=0;
			//Si trame FFT initiale détecté, commence à mettre à jour la FFT
			if((read3&0b00010000)==0x10)
			{
				compteur_fft=1; 
			}
			//Met à jour chaque trame de la FFT
			switch(compteur_fft)
					{
						case 1:
							fft1=(read3&0b00001111);
						break;
						case 2:
							fft2=(read3&0b00001111);
						break;
						case 3:
							fft3=(read3&0b00001111);
						break;
						case 4:
							fft4=(read3&0b00001111);
						break;
						case 5:
							fft5=(read3&0b00001111);
						break;
						case 6:
							fft6=(read3&0b00001111);
						break;
						case 7:
							fft7=(read3&0b00001111);
						break;
						case 8:
							fft8=(read3&0b00001111);
						break;
						case 9:
							fft9=(read3&0b00001111);
						break;
						case 10:
							fft10=(read3&0b00001111);
						break;
						case 11:
							fft11=(read3&0b00001111);
						break;
						case 12:
							fft12=(read3&0b00001111);
						break;
						case 13:
							fft13=(read3&0b00001111);
						break;
						case 14:
							fft14=(read3&0b00001111);
						break;
						case 15:
							fft15=(read3&0b00001111);
						break;
						case 16:
							fft16=(read3&0b00001111);
							compteur_fft=0;
						break;
						default:
							LED2=1;
						break;
					}	
			//Lorsque la FFT est actualiser, la faire afficher à l'écran
			if(compteur_fft==1 && (emplacement ==1)  && (etatplay==1))
				graphFFT(2*fft1,2*fft2,2*fft3,2*fft4,2*fft5,2*fft6,2*fft7,2*fft8,2*fft9,2*fft10,2*fft11,2*fft12,2*fft13,2*fft14,2*fft15,2*fft16);	
			compteur_fft++;
			//Choix de la chanson selon les données reçu
			if((read4&0b00001111)==0b00000001)
				toune=1;
			else if((read4&0b00001111)==0b00000010)
				toune=2;
			else if((read4&0b00001111)==0b00000011)
				toune=3;
			//Masques pour savoir les actuateurs à activer
			i2c_actuateur=0b00001000;
			if(vu && !pause)
				i2c_actuateur |= 0b00000001;			
			if(proj && !pause)
				i2c_actuateur |= 0b00000010;
			if(strob && !pause)
				i2c_actuateur |= 0b00000100;
			
			//Émet I2C uniquement sur nouvelles données
			if((last_i2c_1 != read2) || (last_i2c_2 != i2c_actuateur))
			{
				tst_str[1] = read2;
				tst_str[2] = i2c_actuateur;
				send_str_I2C(0x40, tst_str);
			}
			last_i2c_1 = read2;		
			last_i2c_2 = i2c_actuateur;	
		}
		if(play)
		{
			while(BusyUSART());
			putcUSART(euart_out);
			play=0;
		}
	}
} 

//Configuration du PIC
#pragma config PLLDIV = 1, CPUDIV = OSC1_PLL2, USBDIV = 2
#pragma config FOSC = HSPLL_HS, FCMEN = OFF, IESO = OFF
#pragma config VREGEN = ON, PWRT = OFF, BOR = ON, BORV = 2
#pragma config WDT = OFF, WDTPS = 32768
#pragma config CCP2MX = ON, PBADEN = OFF, LPT1OSC = OFF, MCLRE = ON
#pragma config STVREN = ON, LVP = OFF, XINST = OFF

void barrgraph(short xpos, short ypos, short width, short value, short step) //Affiche un ligne gradué de 0 à 24 pixel 
{	
	unsigned char A;
	unsigned char B;
	unsigned char C;
	char tab[];
	short i = 0;
	if(value < 8){
		A = 0x00;
		B = 0x00;
		C = value2graph(value);}
	if(value >= 8 && value < 16){
		A = 0x00;
		B = value2graph(value - 8);
		C = 0xFF;}
	if(value >= 16){
		A = value2graph(value - 16);
		B = 0xFF;
		C = 0xFF;}
	for(i = 0; i < width; i++)
		{
			if(step == 0){
				GLCD_GoTo(xpos+i,ypos);
				GLCD_WriteData(A);
				GLCD_GoTo(xpos+i,ypos+1);
				GLCD_WriteData(B);
				GLCD_GoTo(xpos+i,ypos+2);
				GLCD_WriteData(C);}
			if(step == 1){
				GLCD_GoTo(xpos+i,ypos);
				GLCD_WriteData(A&0xAA);
				GLCD_GoTo(xpos+i,ypos+1);
				GLCD_WriteData(B&0xAA);
				GLCD_GoTo(xpos+i,ypos+2);
				GLCD_WriteData(C&0xAA);}
		}	
}
short value2graph(short value) //Convertis une valeur entre 0 à 8 en niveau graphique en pixel
{
	short sortie = 0;
	if (value <1)
		sortie = 0x00;
	if (value >= 1 && value < 2)
		sortie = 0x80;
	if (value >= 2 && value < 3)
		sortie = 0xC0;
	if (value >= 3 && value < 4)
		sortie = 0xE0;
	if (value >= 4 && value < 5)
		sortie = 0xF0;
	if (value >= 5 && value < 6)
		sortie = 0xF8;
	if (value >= 6 && value < 7)
		sortie = 0xFC;
	if (value >= 7 && value < 8)
		sortie = 0xFE;
	if (value >= 8)
		sortie = 0xFF;
	return sortie;
}
void graphFFT(short val_1, short val_2,short val_3,short val_4,short val_5,short val_6,short val_7,short val_8,short val_9, short val_10,short val_11,short val_12,short val_13,short val_14,short val_15,short val_16)
{
	short x = 24;
	barrgraph(0+x,2, 3, val_1,0);
	barrgraph(5+x,2, 3, val_2,0);
	barrgraph(10+x,2, 3, val_3,0);
	barrgraph(15+x,2, 3, val_4,0);
	barrgraph(20+x,2, 3, val_5,0);
	barrgraph(25+x,2, 3, val_6,0);
	barrgraph(30+x,2, 3, val_7,0);
	barrgraph(35+x,2, 3, val_8,0);
	barrgraph(40+x,2, 3, val_9,0);
	barrgraph(45+x,2, 3, val_10,0);
	barrgraph(50+x,2, 3, val_11,0);
	barrgraph(55+x,2, 3, val_12,0);
	barrgraph(60+x,2, 3, val_13,0);
	barrgraph(65+x,2, 3, val_14,0);
	barrgraph(70+x,2, 3, val_15,0);
	barrgraph(75+x,2, 3, val_16,0);
}
void graphVU(short val_L,short val_R)
{
	barrgraph(0,2, 17, val_L,1);
	barrgraph(111,2, 17, val_R,1);
}

