#ifndef INC_DEF_H
#define INC_DEF_H

#include <p24fj16GA004.h>
#include <stdlib.h>
#include <stdio.h>
#include <uart.h>
#include <timer.h>
#include <adc.h>
#include <libpic30.h>
#include "periph.h"
#include "usart.h"
#include "KS0108.h"
#include "graphic.h"
#include "commRF.h"
#include "fifo.h"

#define FCY 32000000

//Configuration du module:
#define BORNE				//Borne
//#define AUTO				//Auto. Pour le moment, Auto = RX
//#define USE_GLCD 			//Le GLCD va avec l'auto
//#define GPS_FEEDTHROUGH	//Le GPS est avec Auto

#define DEBUG_MPSIM


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  	Function prototypes                                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void setup_oscillator(void);
void config(void);
void demo_numerisation(void);
char get_offset(unsigned char msb, unsigned char lsb, unsigned char ref);
void clean_buffer(unsigned char offset, char buf_length);
unsigned int get_rssi(void);
unsigned char get_flag(unsigned char flag);
void switchScreen(int);
void switchBatt(void);
void getBatt(void);

/////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         //
//                                          Definitions                                    //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

#define ADC_RSSI 	0
#define ADC_POT		1

#define AVG 16

//Pins:
//======

//LED
#define ALIVE			PORTBbits.RB13
#define TRIS_ALIVE		TRISBbits.TRISB13

//Analogique
#define TRIS_RSSI		TRISAbits.TRISA0
#define TRIS_POT		TRISAbits.TRISA1

//Encodeur
#define ENC_SW			PORTBbits.RB7
#define TRIS_ENC_SW		TRISBbits.TRISB7
#define ENC_SWA			PORTBbits.RB4
#define TRIS_ENC_SWA	TRISBbits.TRISB4
#define ENC_SWB			PORTBbits.RB6
#define TRIS_ENC_SWB	TRISBbits.TRISB6

//UART PC (UART-USB)
#define TRIS_RXUSB 		TRISBbits.TRISB15	//Sortie
#define TRIS_TXUSB 		TRISBbits.TRISB14	//Entrée

//UART GPS
#define TRIS_RXGPS 		TRISBbits.TRISB2	//Sortie
#define TRIS_TXGPS 		TRISBbits.TRISB3	//Entrée
#define PPS				PORTAbits.RA4
#define TRIS_PPS		TRISAbits.TRISA4

//UART Radio (Combiné RX et TX)
#define TRIS_TXRX 		TRISBbits.TRISB9	//Mix
#define TR				LATBbits.LATB8
#define TRIS_TR			TRISBbits.TRISB8

//KS0108 GLCD
#define DATA 				PORTC			//Attention, seuls les 8 LSB!
#define TRIS_DATA 			TRISC

#define KS0108_EN			LATAbits.LATA2
#define TRIS_KS0108_EN		TRISAbits.TRISA2

#define KS0108_CS1			LATCbits.LATC9
#define TRIS_KS0108_CS1		TRISCbits.TRISC9

#define KS0108_CS2			LATCbits.LATC8
#define TRIS_KS0108_CS2		TRISCbits.TRISC8

#define KS0108_RW			LATAbits.LATA3
#define TRIS_KS0108_RW		TRISAbits.TRISA3

#define KS0108_RS			LATAbits.LATA8
#define TRIS_KS0108_RS		TRISAbits.TRISA8

#define KS0108_RST			LATBbits.LATB12
#define TRIS_KS0108_RST		TRISBbits.TRISB12

//Module radio
#define TRM_TX	1
#define TRM_RX	0

//Paramètres de la couche de liaison de données
#define FANION 			0xFF
#define NBRFANION 		8
#define NBRFF           8
#define NBROCTET        1
#define ADRESSE 		0xA0
#define CONFIRMATION 	0xAA
#define ATTENTE_CONF 	400		//millisecondes

#endif
