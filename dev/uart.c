#include "uart.h"
#include "interrupt.h"
#include "comm.h"
#include "beep.h"
#include "led.h"

#define		rGPHCON		(*(volatile unsigned long*)0x56000070)
void uart_init()
{
		rUFCON0 = 0x0;   //UART channel 0 FIFO control register, FIFO disable
		rUMCON0 = 0x0;   //UART chaneel 0 MODEM control register, AFC disable
		rULCON0 = 0X03;
	//	rUCON0 |= (0X01 << 2);
	//	rUCON0 |= 0x01;
		rUCON0 = 0x385;
		
		//设置波特率  UBRDIV n = (int)( UART 时钟  / (  波特率  × 16) )  – 1  
		rUBRDIV0 = (int)(PCLK/(BAUD*16)-1);	

		rGPHCON &= ~(0xf << 4); 
		rGPHCON |= (0xa << 4);
		
		EnableIrq(BIT_UART0);
		EnableSubIrq(BIT_SUB_RXD0);
		ClearPending(BIT_UART0);
		ClearSubPending(BIT_SUB_RXD0);
		ClearSubPending(BIT_SUB_TXD0);
}

#define		PCLK		50000000
#define		BAUD		115200
void uart_dma_init()
{
		//配置引脚功能
		rGPHCON &= ~(0xf << 4); 
		rGPHCON |= (0xa << 4);
		
		//设置数据格式
		rULCON0 = 0x3;
		
		//设置工作模式   DMA模式
		rUCON0 = 0x9;
		
		//设置波特率  UBRDIV n = (int)( UART 时钟  / (  波特率  × 16) )  – 1  
		rUBRDIV0 = (int)(PCLK/(BAUD*16)-1);		
}

//Uart打印 printf.c文件存放在lib目录下
void send_byte(unsigned char dat)
{
		while( !(rUTRSTAT0 & (1<<2)) );	  
				rUTXH0 = dat;		
}



unsigned char rece_byte()
{
		unsigned char ret;	
		
		while( !(rUTRSTAT0 & (1<<0)) );	
				
		ret = rURXH0;
				
		if( ret == 0x0d ) {   //实现回显
			
				send_byte(0x0d);		//回车
				send_byte(0x0a);		//换行
				
		} else { 
			
				send_byte(ret);	
		}			
	
		return ret;
}


int send_string(const char* buff)
{
	int i = 0;
		
	send_byte(0x0d);		//回车（光标移到最左）
	send_byte(0x0a);		//换行（跳到下一行）
		
	while( buff[i] ) {
			
		send_byte(buff[i++]);	
	}
		
	return --i;
}

void uart0_isr(void)
{
	unsigned char err;
	unsigned int c;

	if(rUTRSTAT0 & 0x1) 		                              
	{
		c = RdURXH0();
		CommPutRxChar(COM1, (unsigned char)c);
		ClearSubPending(BIT_SUB_RXD0);
	}
	else if((rUTRSTAT0 & 0x2))			                
	{
		c = CommGetTxChar(COM1, &err);/* Get next character to send.            */
		if (err == COMM_TX_EMPTY)     /* Do we have anymore characters to send  */	                           
			DisableSubIrq(BIT_SUB_TXD0);
		else             
			WrUTXH0(c);            	/* Insert received character into buffer  */
		ClearSubPending(BIT_SUB_TXD0);
	}

	ClearPending(BIT_UART0);
}

