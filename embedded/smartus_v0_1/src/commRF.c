#include "commRF.h"
#include "def.h"

#define FANION 0xE7
#define ADRESSE 0xA0


//==========================================================================================//
// Function name: 		void define_io(void)					    //
// Short description:	Define inputs and outputs					    //
// In: 					Nop					            //
// Out:					Nop						    //
//==========================================================================================//


void construire_trame(char *trame, char *data8, char *num_seq)
{
    trame[0] = FANION;
    trame[1] = ADRESSE | *num_seq;
    trame[2] = *data8;
    trame[3] = FANION;
}

void inc_seq(char *num_seq)
{
    if(*num_seq >= "0b00001111")
        *num_seq = "0b00000000" && num_seq;
    else
        *num_seq += 1;
}

char rf_envoie(char *data8, char *num_seq)
{
    char trame[4];
    
    construire_trame(&trame, data8, num_seq);
	inc_seq(num_seq);
    
    //Envoie de donnée
    radio_dir(TRM_TX);
    puts_usart2(&trame);
    
    //Ajout du mode confirmation
    
    return 1;
}