//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                   [Projet S5P06 - UdeS]                                  //
//                                Librairie universelle KS0108                              //
//                                       version 1.0                                        //
//																		                    //
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                              PIC18F4550, 4MHz x PLL = 10MIPS                             //
//                   (c)Rados³aw Kwiecieñ (radek@dxp.pl), modifié par JFDuval               //
//                                        30/10/2010         		                        //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#include "def.h"
#include "KS0108-PIC24.h"
#include "KS0108.h"
#include "font5x8.h"

extern void GLCD_InitalizePorts(void);

unsigned char screen_x = 0, screen_y = 0;
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_Initalize(void)
{
	unsigned char i;
	GLCD_InitializePorts();
	
	for(i = 0; i < 3; i++)
	{
		GLCD_WriteCommand((DISPLAY_ON_CMD | GLCD_ON), i);
	}
}
//-------------------------------------------------------------------------------------------------
// Positionne le curseur, x =  largeur et y = hauteur
//-------------------------------------------------------------------------------------------------
void GLCD_GoTo(unsigned char x, unsigned char y)
{
	unsigned char i;
	screen_x = x;
	screen_y = y;
	
	for(i = 0; i < KS0108_SCREEN_WIDTH/64; i++)
  	{
	  	GLCD_WriteCommand(DISPLAY_SET_Y | y,i);
  		GLCD_WriteCommand(DISPLAY_SET_X | 0,i);
  		
  		GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
 	}
 	GLCD_WriteCommand(DISPLAY_SET_Y | y, (x / 64));
	GLCD_WriteCommand(DISPLAY_SET_X | (x % 64), (x / 64));
}

//-------------------------------------------------------------------------------------------------
// Vide l'écran (tout bleu)
//-------------------------------------------------------------------------------------------------
void GLCD_ClearScreen(void)
{
	unsigned char i, j;
	
	for(j = 0; j < KS0108_SCREEN_HEIGHT/8; j++)
  	{
  		GLCD_GoTo(0,j);
  		
  		for(i = 0; i < KS0108_SCREEN_WIDTH; i++)  	
  		{	
    		GLCD_WriteData(0x00);
    	}	
  	}
}

//-------------------------------------------------------------------------------------------------
// Remplis l'écran (tout blanc)
//-------------------------------------------------------------------------------------------------
void GLCD_FillScreen(void)
{
	unsigned char i, j;
	
	for(j = 0; j < KS0108_SCREEN_HEIGHT/8; j++)
  	{
  		GLCD_GoTo(0,j);
  		
  		for(i = 0; i < KS0108_SCREEN_WIDTH; i++)  	
  		{	
    		GLCD_WriteData(0xFF);
    	}	
  	}
}
//-------------------------------------------------------------------------------------------------
// Écris un caractère. Penser à faire un Goto avant pour le positionner
//-------------------------------------------------------------------------------------------------
void GLCD_WriteChar(char charToWrite)
{
	int i = 0, index = 0;
	charToWrite -= 32; 
	
	for(i = 0; i < 5; i++) 
	{
		index = (5*((int)charToWrite)) + i;
	  	GLCD_WriteData(font5x8[index]); 
	}
	
	GLCD_WriteData(0x00);
}

//-------------------------------------------------------------------------------------------------
// Écris une chaîne de caractères. Penser à faire un Goto avant pour le positionner
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * stringToWrite)
{
	while(*stringToWrite)
	{
		GLCD_WriteChar(*stringToWrite++);
	}
}

//-------------------------------------------------------------------------------------------------
// 1 = blanc, x =  largeur et y = hauteur
//-------------------------------------------------------------------------------------------------
void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
	unsigned char tmp;
	
	GLCD_GoTo(x, (y / 8));
	tmp = GLCD_ReadData();
	GLCD_GoTo(x, (y / 8));
	tmp = GLCD_ReadData();
	GLCD_GoTo(x, (y / 8));
	tmp |= (1 << (y % 8));
	GLCD_WriteData(tmp);
}

//-------------------------------------------------------------------------------------------------
// Charge un bitmap
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
	unsigned char i, j;
	
	for(j = 0; j < dy / 8; j++)
	{
		GLCD_GoTo(x,y + j);
		
		for(i = 0; i < dx; i++) 
		{
			GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp++));
		}
	}
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------





