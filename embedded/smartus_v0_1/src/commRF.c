#include "commRF.h"
#include "def.h"

//variables
extern volatile unsigned rf_rx_flag;
extern volatile unsigned rf_delai_flag;
extern unsigned int rssi_flag;
extern unsigned char fifo[];
extern unsigned char clean_buf[32];
extern unsigned int rssi;
extern unsigned int adc_rssi[];
extern volatile unsigned int rf_flag;

//Transmission**********************************************************************
char rf_envoie(char *donnee8, char *tampon)
{

    char trame[NBRFF+NBRFANION*2+NBROCTET];
	char trame_de_confirmation[NBROCTET] ;
	unsigned char result =10;
	unsigned int i, j, cnt =0;

	construire_trame_envoie(trame, donnee8);
	
    while(cnt < NBR_ESSAIE)
    {
	//Envoie de donnée
	radio_dir(TRM_TX);
	delay_us(TEMP_SWITCH_RX_TO_TX);

	__asm__ volatile ("disi #0x3FFF"); //disable interrupts

	for(i=0; i < NBRFF+NBRFANION*2+NBROCTET; i++)
	{
	    while(busy_usart2());
		U2TXREG = trame[i] & 0xFF;
	}

	__asm__ volatile ("disi #0x000"); //Enable interrupts

	#ifdef DEBUG_RF
	for(i=0; i < NBRFF+NBRFANION*2+NBROCTET; i++)
	{
		while(busy_usart1());
		    U1TXREG = trame[i] & 0xFF;
	}
	#endif
    
    
	//Retour en mode réception
	radio_dir(TRM_RX);
	delay_us(TEMP_SWITCH_TX_TO_RX);
	
	//Remise à zéro immédiate du flag RSSI
	rssi_flag = 0;
	result = 10;
	rf_delai_flag = 0;
	i=0;
	rf_flag = 0;
	
	
	//Attente de confirmation 
	//Recherche d'une trame de confirmation
	while(result == 10 && i < ATT_CONF_NBR_500)
	{
		if(rssi_flag)
		{
			rssi_flag = 0;
			//Filtre les données quand le buffer est plein
			rssi = get_rssi();
		}
		if(rf_flag)
		{
			result = get_flag(FANION);
		}
		if( rf_delai_flag == 1)
		{
			    i =i+1;
			    rf_delai_flag =0;
		}
	}
	
	if(result != 10)
	{
		rssi =0;
		clean_buffer(result, 32);
	    //Lire trame et confirmer la réception
	    rf_extraction((char *)clean_buf, trame_de_confirmation);
	    
	    char test = CONFIRMATION;
	    for(i =0; i < NBROCTET; i++)
	    {
		if(trame_de_confirmation[i] == test)
		{
			for(j = 0; j < NBROCTET ; j++)
			{
				donnee8[j] = 0;
			}
		Nop();
		return 1;
		}
	    }
	    
	}
	cnt = cnt+1;
    }
	return 0;  
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
	for(i; i < (NBRFF+NBRFANION); i++)
	{
		trame[i] = FANION;
	}
	
	//Ajout des données utiles à la couche application
	for(i; i < (NBRFF+NBRFANION+NBROCTET); i++)
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
		
		construire_trame_envoie(trame, donnee_confirmation);

		radio_dir(TRM_TX);
	
		delay_us(TEMP_SWITCH_RX_TO_TX);
		
		__asm__ volatile ("disi #0x3FFF"); //disable interrupts
		
		for(i=0; i < NBRFF+NBRFANION*2+NBROCTET; i++)
		{
			while(busy_usart2());
			    U2TXREG = trame[i] & 0xFF;
		}
		
		__asm__ volatile ("disi #0x000"); //Enable interrupts
	
		#ifdef DEBUG_RF
		for(i=0; i < NBRFF+NBRFANION*2+NBROCTET; i++)
		{
			while(busy_usart1());
			    U1TXREG = trame[i] & 0xFF;
		}
		#endif

		radio_dir(TRM_RX);
   		delay_us(TEMP_SWITCH_TX_TO_RX);

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

unsigned char get_flag(unsigned char flag)
{
	int i = 0;
	char off;
	
	for(i = 0; i < (FIFO_LENGTH-2); i++)
	{
		off = get_offset(fifo[i], fifo[i+1], flag);
		if(off != 10)
			break;
	}
	
	return off;
}

//Retourne de combien de caractères on doit shifter à gauche
//Une valeur de 10 indique "aucun match"

//Bug connu: fausse détection si 0x00 suivi de 0xFF. Sera corrigé sous peu! - JFD

char get_offset(unsigned char msb, unsigned char lsb, unsigned char ref)
{
	int i = 0;
	unsigned int value_16 = (msb << 8) + lsb;
	unsigned int mask = (ref << 8);
	#ifdef DEBUG_MPSIM
	unsigned int temp1 = 0, temp2 = 0;
	#endif
	
	if((msb == 0xFF) || (lsb == 0xFF))
		return 10;	//Impossible que ce soit bon
	
	if((value_16 & 0xFF00) == mask)
		return 0;	//Pas d'offset
	
	for(i = 1; i < 8; ++i)
	{
		mask = mask >> 1;
		#ifdef DEBUG_MPSIM
		temp1 = (value_16 & mask);			
		temp2 = (unsigned int)(ref << (8 - i));
		if(temp1 == temp2)
		{
//			if((i == 7) && (value_16 == 0xFF))
//				return 10;
//			else
				return i;
		}

		else if((value_16 & mask) == (unsigned int)(ref << (8 - i)))
			return i;
		#endif
	}	
	
	//Aucun match!
	return 10;
}

//Assume 2 buffers fixes, celui de réception (fifo[]) et celui nettoyé
//Décale les données et rempli le buffer clean
void clean_buffer(unsigned char offset, char buf_length)
{
	unsigned int temp1 = 0;
	int i = 0;
	
	for(i = 0; i < buf_length-1; i++)
	//for(i = buf_length-1; i > 0; i--)
	{
		//ToDo: Optimiser
		temp1 = (fifo[i] << 8) + fifo[i+1];
		temp1 = temp1 << offset;
		temp1 = (temp1 & 0xFF00) >> 8;		
		
		clean_buf[i] = temp1;
		fifo[i] = 0x00;
	}
}

unsigned int get_rssi(void)
{
	unsigned int i = 0;
	unsigned long sum = 0;
	
	for(i = 0; i < AVG; i++)
	{
		sum += adc_rssi[i];
	}
	
	return (sum >> 4);
}





