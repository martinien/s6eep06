#include "commRF.h"
#include "def.h"

//variables
extern volatile unsigned rf_rx_flag;

//Transmission**********************************************************************
char rf_envoie(char *donnee8)
{
	char trame[NBRFANION+2];
	unsigned int i;

    construire_trame_envoie(&trame[NBRFANION+2], donnee8);
    
    //Envoie de donn�e
    radio_dir(TRM_TX);
    for(i=0; i < NBRFANION+2; i++)
    {
	    while(busy_usart2());
		U2TXREG = trame[i];
    }
    
    //puts_usart2(&trame[NBRFANION+2]);
    
    //Retour en mode r�ception
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

//R�ception************************************************************************

//Validation de la trame de confirmation
char rf_valider_confirmation(char *trame)
{
    //CRC
    
    //Validation
    if(trame[NBRFANION] == CONFIRMATION)
		return 0;
    return 1;
}

//==========================================================================================================================================================================//
// rf_detection_trame
// Description: Synchroniser une trame re�u avec un tableau de char			    					    					    //
// In: 					octet re�u, decalage d�tect�, drapeau signalant que la trame est compl�te, l'adresse de la trame, un compteur			    //
// Out:					d�calage du fanion = valeur retourn�e -1	    										    //
//==========================================================================================================================================================================//

void rf_detection_trame(char *RX, unsigned *decalage, char *trame_complete, char *trame, unsigned *cnt)
{
    if(*decalage != 0)
    {
	
	if(*cnt == 1 && *decalage != 1)
	{
	    //g�n�re un masque de test pour d�tecter les fausses trame
	    char masque = rf_masque_de_test(*decalage-1);
	    char test = *RX & masque;
	    
	    if(test != masque)
	    {
		*cnt = 0;
		*decalage = 0;
		rf_rx_flag = 0;
		return 0;
	    }
	}
	
	
	unsigned int limite = NBRFANION*2+1;
	
	//Ranger les bits dans la trame afin d'obtenir une trame compl�te
	if(*cnt < limite)
	{
	    trame[*cnt] = (*RX << (*decalage-1));
	}
	
	//(if)Pour �viter d'�crire � l'adresse pr�c�dente lorsqu'il n'y pas de synchronisation requie
	if(*decalage != 1)
	{
	    trame[*cnt-1] = trame[*cnt-1] | (*RX >> 8-(*decalage-1)) & rf_decalage_masque(*decalage-1);
	}
	
	//Incr�mentation de la position dans la trame
	*cnt= *cnt+1;
	if(*cnt > limite)
	{
	    *trame_complete = 1;
	    *cnt = 0;
	    *decalage = 0;
	}

    }
    else
    {
	*decalage = rf_recherche_positif(RX);
	if(*decalage != 0)
	{
	    trame[0] = (*RX << (*decalage-1));
	    *cnt= *cnt+1;
	}
    }
    
    //Remise � z�ro du flag du registre de r�ception
    rf_rx_flag = 0;
    
}

//==========================================================================================================================================================================//
// rf_recherche_positif																			    //
// Description: Recherche la possibilit� d'avoir un fanion ou une partie			    					    				    //
// In: 					octet de donn�es de l'uart			    										    //
// Out:					�calage du fanion = valeur retourn�e -1	    										    	    //
//==========================================================================================================================================================================//

unsigned rf_recherche_positif(char *RX)
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

//===============================================================================================================//
// rf_decalage_masque									     			 //
// Description: G�n�re un masque pour les op�rations binaires sur la trame en fct du d�calage  			 //
// Short description:	Define inputs and outputs					    			 //
// In: 					nbr de bit de d�calage				    			 //
// Out:					masque binaire de un octet			    			 //
//===============================================================================================================//

unsigned rf_decalage_masque(unsigned decalage)
{

    switch(decalage)
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


//==========================================================================================//
// Function: G�n�re un masque pour d�tecter les fausses trames  fct du d�calage 	    //
// Short description:	Define inputs and outputs					    //
// In: 					nbr de bit de d�calage				    //
// Out:					masque binaire de un octet			    //
//==========================================================================================//
char rf_masque_de_test(unsigned decalage)
{

    switch(decalage)
    {
	case 1:
	    return 0b10000000;
	    break;
	case 2:
	    return 0b11000000;
	    break;
	case 3:
	    return 0b11100000;
	    break;
	case 4:
	    return 0b01110000;
	    break;
	case 5:
	    return 0b00111000;
	    break;
	case 6:
	    return 0b10011100;
	    break;
	case 7:
	    return 0b11001110;
	    break;
	default:
	    return 0;
	    break;
    }
}

//CRC******************************************************************************

//char rf_crc(char *trame)
//{
//    char buffer[3];
//    unsigned i = 0;
//    
//    //copie de la charge utile
//    buffer[2] = trame[2];
//
//    for(i=0; i < 12; i++)
//    {
//	buffer[2] << i;
//	buffer[2]^=0x98;
//    }
//    
//    
//    
//}




