#ifndef INC_UART_H
#define INC_UART_H

//Important: to use NetworkViewer, keep these function names!

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//                        	Function prototypes                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void setup_usart1(void);
void putc_usart1(char data);
char getc_usart1(void);
char busy_usart1(void);
char datardy_usart1(void);
void puts_usart1(char *data);
void gets_usart1(char *buffer, unsigned char len);

#endif
