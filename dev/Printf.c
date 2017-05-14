
//********************************************************************
//OSPrintf post message to UartQ
//TaskUart send message to Uart from UartQ
//tangxiaofeng xidian 503
//********************************************************************

#include  "app_cfg.h" 
#include "os_cpu.h"
#include "ucos_ii.h"
#include "vsprintf.h"
#include "uart_lib.h"

#define UartMsgNum  25
#define UartMsgBufLengh 100

OS_STK	TaskUartStk[TaskUartStkLengh];       // Define the TaskUart stack 

static OS_MEM *pUartMem;
static INT8U  UartBuf[UartMsgNum][UartMsgBufLengh];
static OS_EVENT *pUart_Q;
static void *MsgUart[UartMsgNum];


void OSPrintfInit(void)
{   
	INT8U err;
	pUartMem=OSMemCreate(UartBuf,UartMsgNum,UartMsgBufLengh,&err);//Create a mem for Uart post Msg
	
	pUart_Q=OSQCreate(&MsgUart[0],UartMsgNum);//Create a Quen for Uart
	
	OSTaskCreate(TaskUart,(void*)0,&TaskUartStk[TaskUartStkLengh-1],TaskUartPrio);//Creat a Task to sent Msg to Uart
}

void OSPrintf(const char *fmt,...)//post massage to UartQ
{
    
    INT8U *pUartBuf;
    INT8U err;
    va_list ap;
    
    pUartBuf=OSMemGet(pUartMem,&err);//Reqire a Mem for Msg
    va_start(ap,fmt);
    vsprintf((char *)pUartBuf,fmt,ap);//Msg formate
    va_end(ap);
    //Uart_SendString(string);
    OSQPost(pUart_Q,pUartBuf);//Post Msg
}
void TaskUart(void *pdata) //send message to uart from uartQ
{
  
	INT8U *pUartMsg;
	INT8U err;
	pdata=pdata;
	
	
	
	while(1)
	{
	    pUartMsg=OSQAccept(pUart_Q,&err);//Accept the Msg from Qene; no wait 
		while(pUartMsg)
			{
				Uart_SendString((char *)pUartMsg);//seng str to Uart
				OSMemPut(pUartMem,pUartMsg);
				pUartMsg=OSQAccept(pUart_Q,&err);//free Mem
			}
		
		
		OSTimeDly(OS_TICKS_PER_SEC/5);

    }
}



