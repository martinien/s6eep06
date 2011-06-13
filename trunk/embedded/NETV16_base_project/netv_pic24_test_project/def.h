#ifndef INC_DEF_H
#define INC_DEF_H

#include <p24f16ka101.h>
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
#define TRIS_RX		TRISBbits.TRISB2
#define TRIS_TX		TRISBbits.TRISB7

#endif
