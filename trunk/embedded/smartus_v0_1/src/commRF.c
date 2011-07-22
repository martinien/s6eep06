#include "commRF.h"
#include "def.h"

//Paramètres de la couche de liason de données
#define FANION 0xE7
#define NBRFANION 1
#define ADRESSE 0xA0
#define CONFIRMATION 0xAA
#define ATTENTE_CONF 400	//milliseconde


//==========================================================================================//
// Function name: 		void define_io(void)					    //
// Short description:	Define inputs and outputs					    //
// In: 					Nop					            //
// Out:					Nop						    //
//==========================================================================================//


void construire_trame_envoie(char *trame, char *donnee8)
{
int i =0;
    for(i=0; i<NBRFANION ; i++)
	{
		trame[i] = FANION;
	}

    trame[i] = *donnee8;
    trame[i+1] = FANION;
}

//void inc_seq(char *num_seq)
//{
//    if(*num_seq >= "0b00001111")
//        *num_seq = "0b00000000" && num_seq;
//    else
//        *num_seq += 1;
//}

//Envoie normale
char rf_envoie(char *donnee8)
{
	char trame[NBRFANION+2];

    construire_trame_envoie(&trame[NBRFANION+2], donnee8);
    
    //Envoie de donnée
    radio_dir(TRM_TX);
    puts_usart2(&trame[NBRFANION+2]);
    
    
    //Ajout du mode confirmation
    //radio_dir(TRM_RX);
    
       
    return "REUSSIE";
}

// Envoie de confirmation