#include "uart_lib.h"
#include "uart.h"
#include "vsprintf.h"
#include "beep.h"


static int whichUart=0;

void Uart_SendByte(int data)
{
	if(whichUart==0)
	{
		if(data=='\n')
		{
			while(!(rUTRSTAT0 & 0x2));
			// Delay(1);                 //because the slow response of hyper_terminal 
			WrUTXH0('\r');
		}
		while(!(rUTRSTAT0 & 0x2));   //Wait until THR is empty.
		//  Delay(1);
		WrUTXH0(data);
	}
	else if(whichUart==1)
	{
		if(data=='\n')
		{
			while(!(rUTRSTAT1 & 0x2));
			//Delay(1);                 //because the slow response of hyper_terminal 
			rUTXH1 = '\r';
		}
		while(!(rUTRSTAT1 & 0x2));   //Wait until THR is empty.
		//Delay(1);
		rUTXH1 = data;
	}   
	else if(whichUart==2)
	{
		if(data=='\n')
		{
			while(!(rUTRSTAT2 & 0x2));
			//Delay(1);                 //because the slow response of hyper_terminal 
			rUTXH2 = '\r';
		}
		while(!(rUTRSTAT2 & 0x2));   //Wait until THR is empty.
		//Delay(1);
		rUTXH2 = data;
	}       
} 

void Uart_SendString(char *pt)
{
    while(*pt)
        Uart_SendByte(*pt++);
}

void Uart_Printf(char *fmt,...)
{

	// beep_falsh();
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Uart_SendString(string);
	va_end(ap);
}