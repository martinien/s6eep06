#include "commRF.h"
#include "def.h"

//variables
extern volatile unsigned rf_rx_flag;
extern volatile unsigned rf_delai_flag;

//Transmission**********************************************************************
char rf_envoie(char *donnee8, char *tampon)
{
	char trame[NBRFF+NBRFANION*2+NBROCTET];
	char trame_de_confirmation[NBROCTET] ;
	unsigned char result =10;
	unsigned int i;

    construire_trame_envoie(&trame, donnee8);
    
    //Envoie de donnée
    radio_dir(TRM_TX);
	
	delay_us(500);

	__asm__ volatile ("disi #0x3FFF"); //disable interrupts

    for(i=0; i < NBRFF+NBRFANION*2+NBROCTET; i++)
    {
	    while(busy_usart2());
		U2TXREG = trame[i] & 0xFF;
    }

	__asm__ volatile ("disi #0x000"); //Enable interrupts

    for(i=0; i < NBRFF+NBRFANION*2+NBROCTET; i++)
    {
	    while(busy_usart1());
		U1TXREG = trame[i] & 0xFF;
    }
    
    //puts_usart2(&trame[NBRFANION+2]);
    
    //Retour en mode réception
    radio_dir(TRM_RX);
    delay_us(180);
    
    result = 10;
    rf_delai_flag = 0;
    i=0;
    
    //Attente de confirmation  //Trouver une méthode de compter le temps
    //Recherche d'une trame de confirmation
//    while(result == 10 && i < ATT_CONF_NBR_500)
//    {
//	result = get_flag(flag);
//	
//	if( rf_delai_flag == 1)
//	{
//		i =i+1;
//	}
//    }
//    
//    if(result != 10)
//    {
//	//Lire trame et confirmer la réception
//	rf_extraction(tampon, donnee)
//    }
    
    
    return 1;
}

void construire_trame_envoie(char *trame, char *donnee8)
{
	int i =0;
	
	//Octets de réveil
	for(i=0; i<NBRFF ; i++)
	{
		trame[i] = 0xFF;
	}
	
	//Fanion
	for(i; i < NBRFF+NBRFANION; i++)
	{
		trame[i] = FANION;
	}
	
	//Ajout des données utiles à la couche application
	for(i; i < NBRFF+NBRFANION+NBROCTET; i++)
	{
		trame[i] = donnee8[i-(NBRFF+NBRFANION)];
	}
	
	trame[i] = FANION;
}
// Envoie de confirmation

//Réception************************************************************************

//Validation de la trame de confirmation
char rf_gerer_RX(char *tampon, char *donnee)
{
	unsigned int i = 0, trame_valide = 0;
	char donnee_confirmation[NBROCTET];
	char trame[NBRFF+NBRFANION*2+NBROCTET];
	
	//Recherche des données
	trame_valide = rf_extraction(tampon, donnee);
	
	
	//CRC
	//À faire
	
	
	if(trame_valide)
	{
		//envoie de la confirmation
		for(i=0; i < NBROCTET; i++)
		{
			donnee_confirmation[i] = CONFIRMATION;
		}
		
		construire_trame_envoie(&trame, &donnee_confirmation);
		
		__asm__ volatile ("disi #0x3FFF"); //disable interrupts
		
		for(i=0; i < NBRFF+NBRFANION*2+NBROCTET; i++)
		{
			while(busy_usart2());
			    U2TXREG = trame[i] & 0xFF;
		}
		
		__asm__ volatile ("disi #0x000"); //Enable interrupts
		
		return 1;
	}
	return 0;
}

char rf_extraction(char *tampon, char *donnee)
{
	unsigned int i = 0, j = 0, detecte = 0;
	
	for(i =0; i < (FIFO_LENGTH-NBROCTET); i++)
	{	
		if(detecte == 0)
		{
			if(tampon[i] == FANION)
			{
				detecte = 1;
			}
		}
		else
		{
			if( j < NBROCTET)
			{
				donnee[j] = tampon[i];
				j = j+1;
			}
			else
			{
				//Trame complète -faire sortir de la boucle for
				i = FIFO_LENGTH-NBROCTET;
				return 1;
			}
		}
	}
	return 0;
}





