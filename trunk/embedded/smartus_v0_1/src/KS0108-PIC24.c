//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                   [Projet S5P06 - UdeS]                                  //
//                                    Driver PIC18 KS0108                                   //
//                                       version 1.0                                        //
//																		                    //
//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                              PIC18F4550, 4MHz x PLL = 10MIPS                             //
//                   (c)Rados³aw Kwiecieñ (radek@dxp.pl), modifié par JFDuval               //
//                                        30/10/2010         		                        //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

#include "KS0108-PIC24.h"
#include "def.h"
#include <libpic30.h>

#define DISPLAY_STATUS_BUSY 0x80

extern unsigned char screen_x, screen_y;
//-------------------------------------------------------------------------------------------------
// Delay function
//-------------------------------------------------------------------------------------------------
void GLCD_Delay(void)
{
	//Nop();
	delay_us(10);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void)
{
	DATA &= 0xFF00;
	KS0108_EN = 0;
	KS0108_RS = 0;
	KS0108_RW = 0;
	KS0108_RST = 1;			//Was 0
	KS0108_CS1 = 0;
	KS0108_CS2 = 0;
	
	TRIS_DATA &= 0xFF00;	
	TRIS_KS0108_EN = 0;
	TRIS_KS0108_CS1 = 0;
	TRIS_KS0108_CS2 = 0;
	TRIS_KS0108_RW = 0;
	TRIS_KS0108_RS = 0;
	TRIS_KS0108_RST = 0;	//Was 1
}
//-------------------------------------------------------------------------------------------------
// Enable Controller (0-1)
//-------------------------------------------------------------------------------------------------
void GLCD_EnableController(unsigned char controller)
{
	switch(controller)
	{
		case 0 : 
			KS0108_CS1 = 1;
			KS0108_CS2 = 0;
			break;
		case 1 : 
			KS0108_CS1 = 0;
			KS0108_CS2 = 1;
			break;
		default:
			KS0108_CS1 = 0;
			KS0108_CS2 = 0;
			break;
	}
}
//-------------------------------------------------------------------------------------------------
// Disable Controller (0-1)
//-------------------------------------------------------------------------------------------------
void GLCD_DisableController(unsigned char controller)
{
	switch(controller)
	{
		case 0 : 
			KS0108_CS1 = 0;
			KS0108_CS2 = 0;
			break;
		case 1 : 
			KS0108_CS1 = 0;
			KS0108_CS2 = 0;
			break;
		default:
			KS0108_CS1 = 0;
			KS0108_CS2 = 0;
			break;
	}
}
//-------------------------------------------------------------------------------------------------
// Read Status from specified controller (0-2)
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadStatus(unsigned char controller)
{
	unsigned char status;
	
	TRIS_DATA |= 0x00FF;

	KS0108_RW = 1;	
	KS0108_RS = 0;	
	
	GLCD_EnableController(controller);
	
	KS0108_EN = 1;
	GLCD_Delay();
	KS0108_EN = 0;
	GLCD_Delay();
	
	
	KS0108_EN = 1;
	GLCD_Delay();
	status = (DATA & 0xFF);
	KS0108_EN = 0;
	GLCD_DisableController(controller);
	
	#ifdef USE_GLCD
	return status;
	#else
	return 0;
	#endif
}

//-------------------------------------------------------------------------------------------------
// Write command to specified controller
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller)
{
	while(GLCD_ReadStatus(controller) & DISPLAY_STATUS_BUSY);
		
	TRIS_DATA &= 0xFF00;
	KS0108_RW = 0;	
	KS0108_RS = 0;
	KS0108_EN = 1;
	GLCD_EnableController(controller);
	DATA = (commandToWrite & 0xFF);	
	delay_us(10);	//Delay100TCYx(1);	//Réduire?
	KS0108_EN = 0;
	GLCD_DisableController(controller);
}

//-------------------------------------------------------------------------------------------------
// Read data from current position
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadData(void)
{
	unsigned char data;
	
	while(GLCD_ReadStatus(screen_x / 64) & DISPLAY_STATUS_BUSY);
	TRIS_DATA |= 0x00FF;
	
	KS0108_RW = 1;	
	KS0108_RS = 1;	
	
	GLCD_EnableController(screen_x / 64);
	GLCD_Delay();
	
	KS0108_EN = 1;
	GLCD_Delay();
	data = (DATA & 0xFF);
	KS0108_EN = 0;
	GLCD_DisableController(screen_x / 64);
	screen_x++;
	
	return data;
}

//-------------------------------------------------------------------------------------------------
// Write data to current position
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(char data)
{
	while(GLCD_ReadStatus(screen_x / 64) & DISPLAY_STATUS_BUSY);
	TRIS_DATA &= 0xFF00;
	
	KS0108_RW = 0;
	KS0108_RS = 1;	
		
	DATA = data;
	GLCD_EnableController(screen_x / 64);
		
	KS0108_EN = 1;
	GLCD_Delay();
	KS0108_EN = 0;
	
	GLCD_DisableController(screen_x / 64);
	screen_x++;
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(char * ptr)
{
  return *(ptr);
}
