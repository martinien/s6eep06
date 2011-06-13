#ifndef INC_DEF_H
#define INC_DEF_H

#include <p33fj32mc304.h>
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

//UART #1
#define TRIS_RX		TRISCbits.TRISC8
#define TRIS_TX		TRISCbits.TRISC9

#endif
