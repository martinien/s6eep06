#include "usart.h"
#include "def.h"


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
	//UART1 config: Fcy = 36.85MHz, 57600 8N1
	
	// configure U1MODE
	U1MODEbits.UARTEN = 0;	// TX, RX DISABLED, ENABLE at end of func
	U1MODEbits.USIDL = 0;	// Continue in Idle
	U1MODEbits.IREN = 0;	// No IR translation
	U1MODEbits.RTSMD = 0;	// Simplex Mode
	U1MODEbits.UEN = 0;		// TX,RX enabled, CTS,RTS not
	U1MODEbits.WAKE = 0;	// No Wake up (since we don't sleep here)
	U1MODEbits.LPBACK = 0;	// No Loop Back
	U1MODEbits.ABAUD = 0;	// No Autobaud
	U1MODEbits.URXINV = 0;	// IdleState = 1
	U1MODEbits.BRGH = 0;	// 16 clocks per bit period
	U1MODEbits.PDSEL = 0;	// 8bit, No Parity
	U1MODEbits.STSEL = 0;	// One Stop Bit
	
	//  U1BRG = (Fcy/(16*BaudRate))-1		(36.85MHz / (16 * 57600) - 1) = 39
	U1BRG = 39;

	U1STAbits.UTXISEL1 = 0;	//Int when Char is transferred (1/2 config!)
	U1STAbits.UTXINV = 0;	//N/A, IRDA config
	U1STAbits.UTXISEL0 = 0;	//Other half of Bit15
	U1STAbits.UTXBRK = 0;	//Disabled
	U1STAbits.UTXEN = 0;	//TX pins controlled by periph
	U1STAbits.UTXBF = 0;	//*Read Only Bit*
	U1STAbits.TRMT = 0;		//*Read Only bit*
	U1STAbits.URXISEL = 0;	//Int. on character recieved
	U1STAbits.ADDEN = 0;	//Address Detect Disabled
	U1STAbits.RIDLE = 0;	//*Read Only Bit*
	U1STAbits.PERR = 0;		//*Read Only Bit*
	U1STAbits.FERR = 0;		//*Read Only Bit*
	U1STAbits.OERR = 0;		//*Read Only Bit*
	U1STAbits.URXDA = 0;	//*Read Only Bit*

	IPC2bits.U1RXIP = 0b100;	// Mid Range Interrupt Priority level
	IPC3bits.U1TXIP = 0b100;	// Mid Range Interrupt Priority level

	IFS0bits.U1TXIF = 0;	// Clear the Transmit Interrupt Flag
	IEC0bits.U1TXIE = 1;	// Enable Transmit Interrupts
	IFS0bits.U1RXIF = 0;	// Clear the Recieve Interrupt Flag
	IEC0bits.U1RXIE = 1;	// Enable Recieve Interrupts

	U1MODEbits.UARTEN = 1;	// And turn the peripheral on
	U1STAbits.UTXEN = 1;
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
