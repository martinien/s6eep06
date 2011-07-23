#include "commRF.h"
#include "def.h"




//==========================================================================================//
// Function name: 		void define_io(void)					    //
// Short description:	Define inputs and outputs					    //
// In: 					Nop					            //
// Out:					Nop						    //
//==========================================================================================//




//void inc_seq(char *num_seq)
//{
//    if(*num_seq >= "0b00001111")
//        *num_seq = "0b00000000" && num_seq;
//    else
//        *num_seq += 1;
//}

//Transmission**********************************************************************
char rf_envoie(char *donnee8)
{
	char trame[NBRFANION+2];

    construire_trame_envoie(&trame[NBRFANION+2], donnee8);
    
    //Envoie de donnée
    radio_dir(TRM_TX);
    puts_usart2(&trame[NBRFANION+2]);
    
    //Retour en mode réception
    radio_dir(TRM_RX);
    
       
    return 1;
}

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
// Envoie de confirmation

//Réception************************************************************************

//Validation de la trame de confirmation
char rf_valider_confirmation(char *trame)
{
    //CRC
    
    //Validation
    if(trame[NBRFANION] == CONFIRMATION)
		return 0;
    return 1;
}

//Detection de trame complète
void rf_detection_trame(char *RX, char *decalage, char *trame_complete, char *trame, char *cnt)
{
    if(*decalage != 0)
    {
	//Ranger les bits dans la trame afin d'obtenir une trame complète
	trame[*cnt] = (*RX << (*decalage-1));
	trame[*cnt-1] = (*RX >> 8-(*decalage-1)) & rf_decalage_masque(*decalage);
	
	//détection de la fin de la trame
	if( *cnt > (NBRFANION+1) && trame[*cnt-1] == FANION)
	{
	    *trame_complete =1;
	}
    }
    else
    {
	*decalage = rf_recherche_positif(RX);
	if(*decalage != 0)
	{
	    trame[0] = (*RX << *decalage-1);
	    ++cnt;
	}
    }
    
}

//==========================================================================================//
// Function: Recherche la possibilité d'avoir un fanion ou une partie			    //
// Short description:	Define inputs and outputs					    //
// In: 					octet de données de l'uart			    //
// Out:					décalage du fanion = valeur retournée -1	    //
//==========================================================================================//
char rf_recherche_positif(char *RX)
{
    if((*RX & 0xE7) == 0xE7)
	return 1;
    if((*RX & 0x73) == 0x73)
	return 2;
    if((*RX & 0x39) == 0x39)
	return 3;
    if((*RX & 0x1C) == 0x1C)
	return 4;
    if((*RX & 0xE) == 0xE)
	return 5;
    if((*RX & 0x7) == 0x7)
	return 6;
    if((*RX & 0x3) == 0x3)
	return 7;
    if((*RX & 0x1) == 0x1)
	return 8;

    return 0;
}

char rf_decalage_masque(char *decalage)
{
    switch(*decalage-1)
    {
	case 1:
	    return 0b00000001;
	    break;
	case 2:
	    return 0b00000011;
	    break;
	case 3:
	    return 0b00000111;
	    break;
	case 4:
	    return 0b00001111;
	    break;
	case 5:
	    return 0b00011111;
	    break;
	case 6:
	    return 0b00111111;
	    break;
	case 7:
	    return 0b01111111;
	    break;
	default:
	    return 0;
		break;
    }
}

//CRC******************************************************************************




