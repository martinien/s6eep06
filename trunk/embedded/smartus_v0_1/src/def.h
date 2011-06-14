#ifndef INC_DEF_H
#define INC_DEF_H

#include <p24fj16GA004.h>
#include <stdlib.h>
#include <uart.h>
#include <timer.h>
#include "periph.h"
#include "usart.h"


//Define DEBUG for torminal output
//#define DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  	Function prototypes                                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void setup_oscillator(void);
void config(void);
void setup_timer1(void);

/////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         //
//                                          Definitions                                    //
//                                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

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
#define DATA PORTC							//Attention, seuls les 8 LSB!
#define TRIS_DATA TRISC

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

#endif
