#include "def.h"

unsigned int full = 0;
//Buffer:
unsigned char fifo[FIFO_LENGTH];

//Initialisation: tout � 0
void fifo_init(void)
{
	unsigned int i = 0;
	
	for(i = 0; i < FIFO_LENGTH; i++)
	{
		fifo[i] = 0;
	}
}

//Ajoute une donn�e
void fifo_add(unsigned char data, unsigned char reset)
{
	static unsigned int position = 0;
	unsigned int i = 0;
	
	if(reset)
	{
		//On remet en condition d'origine
		position = 0;
		full = 0;		
		return;
	}
	else
	{
		//Fonctionnement normal
			
		//Buffer:
		if(position < FIFO_LENGTH)
			position++;
		else
			full = 1;	//Le buffer est plein, � partir de maintenant on doit shifter
						
		//Ajoute la donn�e actuelle au buffer
		if(!full)
		{
			fifo[position] = data;
		}
		else
		{
			//On shift vers la gauche
			for(i = 0; i < FIFO_LENGTH; i++)
			{
				fifo[i] = fifo[i + 1];
			}
			//On met la nouvelle donn�e � la fin
			fifo[FIFO_LENGTH - 1] = data;
		}				
	}
}
