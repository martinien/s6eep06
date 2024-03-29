#ifndef INC_GRAPHIC_H
#define INC_GRAPHIC_H

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  Prototypes de fonctions                                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void GLCD_Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
void GLCD_Rectangle2(unsigned char x, unsigned char y, unsigned char a, unsigned char b);
void GLCD_Circle(unsigned char cx, unsigned char cy ,unsigned char radius);
void GLCD_Line(unsigned int X1,unsigned int Y1,unsigned int X2,unsigned int Y2);

#endif
