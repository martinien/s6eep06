#ifndef _KS0108PIC
#define _KS0108PIC

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  Prototypes de fonctions                                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void GLCD_Delay(void);
void GLCD_InitializePorts(void);
void GLCD_EnableController(unsigned char controller);
void GLCD_DisableController(unsigned char controller);
void GLCD_WriteCommand(unsigned char commandToWrite, unsigned char controller);
void GLCD_WriteData(char data);
unsigned char GLCD_ReadStatus(unsigned char controller);
unsigned char GLCD_ReadData(void);
unsigned char GLCD_ReadByteFromROMMemory(char * ptr);


#endif
