/*
 * uart.h
 *
 * Created: 5/1/2023 10:34:00 AM
 *  Author: acer
 */ 

#include "uart.h"

void USART_init(void)
{
	uint16 baud= 51;
	UBRRH = (uint8_t)(baud>>8); // high nibble bits
	UBRRL = (uint8_t) baud; // low nibble bits	

	UCSRB |=((1<<4)|(1<<3));     //Enable transmit and enable RXENn TXENn
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0) | (1 << URSEL);   // 2. Set the data size and enable writing to UCSRC
	
}

// Data Transmission Sending Frames with 5 to 8 Data Bit
void USART_transmit( uint8_t data )
{
	// Wait for empty transmit buffer
	while(! (UCSRA & (1 << UDRE)));
	;
	// Put data into buffer, sends the data
	UDR = data;
}

// Receiving Frames with 5 to 8 Data Bits
uint8_t USART_receive( void )
{
	// Wait for data to be received
	while ( !(UCSRA & (1<<RXC)) )
	;
	// Get and return received data from buffer
	return UDR;
}

void USART_Transmit_string (char * str)
{
	uint8_t i =0 ;
	while(str[i]!=0)
	{ USART_transmit(str[i]); i++;}
	
}
char * USART_receive_string (uint8_t *string)  //Receive a character until carriage return or newline

{

	uint8_t  i=0,char_input=0;
	

	do

	{

		*(string+i)= USART_receive();

		char_input = *(string+i);

		i++;

	}while((char_input!='\n') && (char_input!='\r'));
	
	i++;
	*(string+i) = '\0';
		int u=0;
		while ((*(string+u)) !=0) u++;
		*(string+(u-1))=0;
	return string;

}