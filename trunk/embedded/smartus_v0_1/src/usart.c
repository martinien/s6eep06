#include "usart.h"
#include "def.h"

//Important: this code compiles but has never been tested on real hardware!

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                    Constants and variables                               //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

char uart_buf[128];	//Caracter buffer

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                           Functions                                      //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

void setup_usart1(void) 
{
	//UART1 config: Fcy = 16MHz, 9600 8N1
	
	U1MODEbits.UARTEN = 0;	//Disable UART1
	//U1MODEbits.UFRZ = 0;	//Module continue in debug
	U1MODEbits.USIDL = 0;	//Module continue in idle
	U1MODEbits.RTSMD = 1;	//Simplex
	//U1MODEbits.ALTIO = 0;	//Use main pins
	U1MODEbits.UEN = 0;		//Enables only RX&TX
	U1MODEbits.WAKE = 1;	//Wake-up enabled
	U1MODEbits.LPBACK = 0;	//No loopback
	U1MODEbits.ABAUD = 0;	//No autobaud
	U1MODEbits.RXINV = 0;	//Idle = 1
	U1MODEbits.BRGH = 0;	//Low speed
	U1MODEbits.PDSEL = 0;	//8N
	U1MODEbits.STSEL = 0;	//1 stop (8N1)
	
	//U1STAbits.UTXISEL = 0;	//Interrupt on any TX
	U1STAbits.UTXINV = 0;	//Idle = 1
	U1STAbits.UTXBRK = 0;	//Disabled
	U1STAbits.UTXEN = 0;	//TX Disabled
	U1STAbits.URXISEL = 0;	//Interrupt on any character
	U1STAbits.ADDEN = 0;	//Address detect disabled
	
	U1BRG = 103;				//103 = 9600, 51 = 19200 (from table)

	//Interrupts on reception only:
	IFS0bits.U1RXIF = 0;
	IFS0bits.U1TXIF = 0;
	IEC0bits.U1TXIE = 0;
	IEC0bits.U1RXIE = 1;
	IPC2bits.U1RXIP = 4;	//Priority of 4 (0 = disabled, 7 = max)
	
	U1MODEbits.UARTEN = 1;	//Enable UART1
	U1STAbits.UTXEN = 1;	//TX Enabled
}

void putc_usart1(char data)
{	
  	U1TXREG = data & 0xFF;  //Write the data byte to the USART1	
}

char getc_usart1(void)
{	
  	return (char)(U1RXREG & 0xFF);  // Return the received data	
}

char busy_usart1(void)
{
  if(!U1STAbits.TRMT) 	// Is the transmit shift register empty
    	return 1;      	// No, return FALSE
  return 0;            	// Return TRUE
}

char datardy_usart1(void)
{
	return(U1STAbits.URXDA);
}

void gets_usart1(char *buffer, unsigned char len)
{
	char i;    						// Length counter
	unsigned char data;
	
	for(i=0;i<len;i++)  			// Only retrieve len characters
	{
		while(!datardy_usart1());	// Wait for data to be received
	
		data = getc_usart1();    	// Get a character from the USART
	                           		// and save in the string
		*buffer = data;
		buffer++;              		// Increment the string pointer
	}
}

void puts_usart1(char *data)
{
	do
	{  // Transmit a byte
		while(busy_usart1());
		putc_usart1(*data);
	} while( *data++ );
}

//UART2: Module Radio

void setup_usart2(void) 
{
	//UART2 config: Fcy = 16MHz, 300 8N1
	
	U2MODEbits.UARTEN = 0;	//Disable UART1
	//U2MODEbits.UFRZ = 0;	//Module continue in debug
	U2MODEbits.USIDL = 0;	//Module continue in idle
	U2MODEbits.RTSMD = 1;	//Simplex
	//U2MODEbits.ALTIO = 0;	//Use main pins
	U2MODEbits.UEN = 0;		//Enables only RX&TX
	U2MODEbits.WAKE = 1;	//Wake-up enabled
	U2MODEbits.LPBACK = 0;	//No loopback
	U2MODEbits.ABAUD = 0;	//No autobaud
	U2MODEbits.RXINV = 0;	//Idle = 1
	U2MODEbits.BRGH = 0;	//Low speed
	U2MODEbits.PDSEL = 0;	//8N
	U2MODEbits.STSEL = 0;	//1 stop (8N1)
	
	//U1STAbits.UTXISEL = 0;	//Interrupt on any TX
	U2STAbits.UTXINV = 0;	//Idle = 1
	U2STAbits.UTXBRK = 0;	//Disabled
	U2STAbits.UTXEN = 0;	//TX Disabled
	U2STAbits.URXISEL = 0;	//Interrupt on any character
	U2STAbits.ADDEN = 0;	//Address detect disabled
	
	U2BRG = 3332;				//300 = 3332 (from table)

	//Interrupts on reception only:
	IFS1bits.U2RXIF = 0;
	IFS1bits.U2TXIF = 0;
	IEC1bits.U2TXIE = 0;
	IEC1bits.U2RXIE = 1;
	IPC7bits.U2RXIP = 4;	//Priority of 4 (0 = disabled, 7 = max)
	
	U2MODEbits.UARTEN = 1;	//Enable UART1
	U2STAbits.UTXEN = 1;	//TX Enabled
}

void putc_usart2(char data)
{	
  	U2TXREG = data & 0xFF;  //Write the data byte to the USART1	
}

char getc_usart2(void)
{	
  	return (char)(U2RXREG & 0xFF);  // Return the received data	
}

char busy_usart2(void)
{
  if(!U2STAbits.TRMT) 	// Is the transmit shift register empty
    	return 1;      	// No, return FALSE
  return 0;            	// Return TRUE
}

char datardy_usart2(void)
{
	return(U2STAbits.URXDA);
}

void gets_usart2(char *buffer, unsigned char len)
{
	char i;    						// Length counter
	unsigned char data;
	
	for(i=0;i<len;i++)  			// Only retrieve len characters
	{
		while(!datardy_usart2());	// Wait for data to be received
	
		data = getc_usart2();    	// Get a character from the USART
	                           		// and save in the string
		*buffer = data;
		buffer++;              		// Increment the string pointer
	}
}

void puts_usart2(char *data)
{
	do
	{  // Transmit a byte
		while(busy_usart2());
		putc_usart2(*data);
	} while( *data++ );
}
