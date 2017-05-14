#include "comm.h"


COMM_RING_BUF  Comm1Buf;
COMM_RING_BUF  Comm2Buf;
COMM_RING_BUF  Comm3Buf;
COMM_RING_BUF  Comm4Buf;
COMM_RING_BUF  Comm5Buf;
COMM_RING_BUF  Comm6Buf;

#ifdef Peripherals
	COMM_RING_BUF  SPI1Buf;
	COMM_RING_BUF  SPI2Buf;
	COMM_RING_BUF  SPI3Buf;
	
	COMM_RING_BUF  I2C2Buf;
	I2C_CTR_TYPE   i2c2ctr;
	IRDA_COMM_TYPE irda_ctr;
#endif


#ifdef SHELL_ENABLE
	INT16U CmdCnt;
	cmd_list_type cmd_list[MaxCmdCnt];
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                  INITIALIZE COMMUNICATIONS MODULE
*
*
* Description : This function is called by your application to initialize the communications module.  You
*               must call this function before calling any other functions.
* Arguments   : none
*********************************************************************************************************
*/
void  CommInit(INT8U ch)
{
    COMM_RING_BUF *pbuf;

    switch (ch){
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break;
#ifdef Peripherals
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
	   case BI2C2:
	   		pbuf     = &I2C2Buf;                      		/* Initialize the ring buffer for i2c2     */
	   		break;
#endif
	   default:
	   		return;
    }
    pbuf->RingBufRxCtr    = 0;
    pbuf->RingBufRxInPtr  = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufRxSem    = OSSemCreate(0);
    pbuf->RingBufTxCtr    = 0;
    pbuf->RingBufTxInPtr  = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
    pbuf->RingBufTxSem    = OSSemCreate(COMM_TX_BUF_SIZE);
    pbuf->InPrintSem      = OSSemCreate(1);
}

/*$PAGE*/
/*
**********************************************************************************************************
*                                    REMOVE CHARACTER FROM RING BUFFER
*
* Description : This function is called by your application to obtain a character from the communications
*               channel.  The function will wait for a character to be received on the serial channel or
*               until the function times out.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COM0
*                           COM1
*               'to'    is the amount of time (in clock ticks) that the calling function is willing to
*                       wait for a character to arrive.  If you specify a timeout of 0, the function will
*                       wait forever for a character to arrive.
*               'err'   is a pointer to where an error code will be placed:
*                           *err is set to COMM_NO_ERR     if a character has been received
*                           *err is set to COMM_RX_TIMEOUT if a timeout occurred
*                           *err is set to COMM_BAD_CH     if you specify an invalid channel number
* Returns     : The character in the buffer (or NUL if a timeout occurred)
*********************************************************************************************************
*/
INT8U  CommGetChar (INT8U ch, INT16U to, INT8U *err) 
{
    INT8U          c=0;
    INT8U          oserr;
    COMM_RING_BUF *pbuf;
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
    cpu_sr = 0;                                  /* Prevent compiler warning                           */
#endif
    switch (ch) 
	{                                          		 /* Obtain pointer to communications channel 					 */
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break; 
#ifdef Peripherals
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
	   case BI2C2:
	   		pbuf     = &I2C2Buf;                      		/* Initialize the ring buffer for i2c2     */
	   		break;
#endif
    default:
             *err = COMM_BAD_CH;
             return (NUL);
    }
    OSSemPend(pbuf->RingBufRxSem, to, &oserr);             /* Wait for character to arrive             */
    if (oserr == OS_ERR_TIMEOUT) {                             /* See if characters received within timeout*/
        *err = COMM_RX_TIMEOUT;                            /* No, return error code                    */ 
        return (NUL);
    } else {
        	OS_ENTER_CRITICAL();
		if(pbuf->RingBufRxCtr) 
		{
			pbuf->RingBufRxCtr--;                              			/* Yes, decrement character count       */		
	        c = *pbuf->RingBufRxOutPtr++;                      	/* Get character from buffer            */
	        if (pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) {     	/* Wrap OUT pointer */
	            pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
	        }
		} 
		else 
		{

		}    
        *err = COMM_NO_ERR;
		OS_EXIT_CRITICAL();
        return (c);
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                  GET TX CHARACTER FROM RING BUFFER
*
*
* Description : This function is called by the Tx ISR to extract the next character from the Tx buffer.
*               The function returns FALSE if the buffer is empty after the character is extracted from
*               the buffer.  This is done to signal the Tx ISR to disable interrupts because this is the
*               last character to send.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COM0
*                           COM1
*               'err'   is a pointer to where an error code will be deposited:
*                           *err is set to COMM_NO_ERR         if at least one character was available
*                                                              from the buffer.
*                           *err is set to COMM_TX_EMPTY       if the Tx buffer is empty.
*                           *err is set to COMM_BAD_CH         if you have specified an incorrect channel
* Returns     : The next character in the Tx buffer or NUL if the buffer is empty.
*********************************************************************************************************
*/

INT8U  CommGetTxChar (INT8U ch, INT8U *err)
{
    INT8U          c;
    COMM_RING_BUF *pbuf;
	OS_CPU_SR  cpu_sr;

    switch (ch) {                                          /* Obtain pointer to communications channel */
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break;
#ifdef Peripherals			  
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
#endif
    default:
             *err = COMM_BAD_CH;
             return (NUL);
    }
    if (pbuf->RingBufTxCtr > 0) {                          /* See if buffer is empty                   */
        OS_ENTER_CRITICAL();
		pbuf->RingBufTxCtr--;                              		 /* No, decrement character count            */  
        c = *pbuf->RingBufTxOutPtr++;                      /* Get character from buffer                */
        if (pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]) {     /* Wrap OUT pointer     */
            pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
        }
				OS_EXIT_CRITICAL();
        OSSemPost(pbuf->RingBufTxSem);                     /* Indicate that character will be sent     */

        *err = COMM_NO_ERR;
        return (c);                                        /* Characters are still available           */
    } else {
        *err = COMM_TX_EMPTY;
        return (NUL);                                      /* Buffer is empty                          */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                          ENABLE TX INTERRUPTS
*
* Description : This function enables transmission of characters.  Transmission of characters is
*               interrupt driven.  If you are using a multi-drop driver, the code must enable the driver
*               for transmission.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COM0
*                           COM1
*********************************************************************************************************
*/


static void  CommTxIntEn (INT8U ch)
{
//	unsigned char c,err;
    switch (ch) {

			case COM1:
						EnableSubIrq(BIT_SUB_TXD0);
			     		break;
			case COM2:
//				 		USART2 -> CR1 |= 0x0080;
			     		break;    
			case COM3:
//				 		USART3 -> CR1 |= 0x0080;
			     		break;  
			case COM4:
//				 		USART4 -> CR1 |= 0x0080;
			     		break;
			case COM5:
//				 		USART5 -> CR1 |= 0x0080;
			     		break;
			case COM6: //irda
//				 		irda_send_enable();	
			     		break;
#ifdef Peripherals
			case BSPI1:
			     		c = CommGetTxChar(BSPI1, &err);                 /* Get next character to send.               */
				 		if(err == COMM_NO_ERR) 
			   				SPI1->DR = c;		 
			     		break;
			case BSPI2:
			     		c = CommGetTxChar(BSPI2, &err);                 /* Get next character to send.               */
				 		if(err == COMM_NO_ERR) 
			   				SPI2->DR = c;		 
			     		break;
			case BSPI3:
			     		c = CommGetTxChar(BSPI3, &err);                 /* Get next character to send.               */
				 		if(err == COMM_NO_ERR) 
			   				SPI3->DR = c;		 
			     		break;
#endif
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            OUTPUT CHARACTER
*
*
* Description : This function is called by your application to send a character on the communications
*               channel.  The function will wait for the buffer to empty out if the buffer is full.
*               The function returns to your application if the buffer doesn't empty within the specified
*               timeout.  A timeout value of 0 means that the calling function will wait forever for the
*               buffer to empty out.  The character to send is first inserted into the Tx buffer and will
*               be sent by the Tx ISR.  If this is the first character placed into the buffer, the Tx ISR
*               will be enabled.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COM0
*                           COM1
*               'c'     is the character to send.
*               'to'    is the timeout (in clock ticks) to wait in case the buffer is full.  If you
*                       specify a timeout of 0, the function will wait forever for the buffer to empty.
* Returns     : COMM_NO_ERR      if the character was placed in the Tx buffer
*               COMM_TX_TIMEOUT  if the buffer didn't empty within the specified timeout period
*               COMM_BAD_CH      if you specify an invalid channel number
*********************************************************************************************************
*/

INT8U  CommPutChar (INT8U ch, INT8U c, INT16U to)
{
    INT8U          oserr;
    COMM_RING_BUF *pbuf;
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
    cpu_sr = 0;                                  /* Prevent compiler warning                           */
#endif
    switch (ch) 
	{                                            /* Obtain pointer to communications channel 		   */
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break;
#ifdef Peripherals
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
#endif
	    default:
	         return (COMM_BAD_CH);
    }
    OSSemPend(pbuf->RingBufTxSem, to, &oserr);             /* Wait for space in Tx buffer              */
    if (oserr == OS_ERR_TIMEOUT) {
        return (COMM_TX_TIMEOUT);                          /* Timed out, return error code             */
    }
    OS_ENTER_CRITICAL();
    pbuf->RingBufTxCtr++;  
    *pbuf->RingBufTxInPtr++ = c;                           /* Put character into buffer                */
    if (pbuf->RingBufTxInPtr == &pbuf->RingBufTx[COMM_TX_BUF_SIZE]){     /* Wrap IN pointer            */
        pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
    }
		OS_EXIT_CRITICAL();                                	   		/* No, increment character count            */
    if (pbuf->RingBufTxCtr == 1) {                         /* See if this is the first character       */
        CommTxIntEn(ch);                                   /* Yes, Enable Tx interrupts                */
    }
    return (COMM_NO_ERR);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                    INSERT CHARACTER INTO RING BUFFER
*
*
* Description : This function is called by the Rx ISR to insert a character into the receive ring buffer.
* Arguments   : 'ch'    is the COMM port channel number and can either be:
*                           COM0
*                           COM1
*               'c'     is the character to insert into the ring buffer.  If the buffer is full, the
*                       character will not be inserted, it will be lost.
*********************************************************************************************************
*/

void  CommPutRxChar (INT8U ch, INT8U c)
{
    COMM_RING_BUF *pbuf;
		OS_CPU_SR  cpu_sr;
    switch (ch){                                          /* Obtain pointer to communications channel */
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break;
#ifdef Peripherals
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
	   case BI2C2:
	   		pbuf     = &I2C2Buf;                      		/* Initialize the ring buffer for i2c2     */
	   		break;
#endif
    default:
             return;
    }
    if (pbuf->RingBufRxCtr < COMM_RX_BUF_SIZE) {           /* See if buffer is full                    */
	    OS_ENTER_CRITICAL();	 
        pbuf->RingBufRxCtr++;                              /* No, increment character count            */
	
        *pbuf->RingBufRxInPtr++ = c;                       /* Put character into buffer                */
        if (pbuf->RingBufRxInPtr == &pbuf->RingBufRx[COMM_RX_BUF_SIZE]) { /* Wrap IN pointer           */
            pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
        }
			OS_EXIT_CRITICAL();
        OSSemPost(pbuf->RingBufRxSem);                     /* Indicate that character was received     */
    }
}
unsigned char  query_txbuf_empty(INT8U ch)
{
    COMM_RING_BUF *pbuf = 0;

    switch (ch){                                          /* Obtain pointer to communications channel */
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break;
#ifdef Peripherals  
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
#endif 
	   default:
	   		break;
    }
	if(pbuf->RingBufTxCtr)	
		 return 0;
	else
		 return 1;
}

unsigned char  query_rxbuf_empty(INT8U ch)
{
    COMM_RING_BUF *pbuf = 0;

    switch (ch){                                          /* Obtain pointer to communications channel */
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break;
#ifdef Peripherals
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
#endif 
	   default:
	   		break;
    }
	if(pbuf->RingBufRxCtr)	
		 return 0;
	else
		 return 1;
}



/**********************************************************************************************************/

#ifdef SHELL_ENABLE
/*
*********************************************************************************************************
--	convert the upper alphabet to the lower style.
*********************************************************************************************************
*/
INT8U tolow(INT8U c)
{
    if((c >= 'A')&&(c <='Z'))
        return c+32;
    else 
		return c;
}
/*
*********************************************************************************************************
--	scan the delimiters input by the user through series com0 or others.
--
*********************************************************************************************************
*/
void  ScanDelimiters(INT8U * InputLine, INT8U * LineIndex, INT16U Which)
{
    for (;*LineIndex < MAX_LINE-1;(*LineIndex)++) 
	{
        switch (InputLine[*LineIndex]) 
		{
            case ' ':
            case '-':
            case ':':
                continue;
            case '=':
                if ((Which & SCAN_EQUALS) != 0)
                    continue;
            break;
            case ',':
                if ((Which & SCAN_COMMA) != 0)
                    continue;
            break;
            case '.':
                if ((Which & SCAN_DOT) != 0)
                    continue;
            break;
        }
        break;
    }
}
/*
*********************************************************************************************************
--
--
*********************************************************************************************************
*/
INT8U strcmp1(char * s1, char * s2)
{
    while (*s1 == *s2) 
	{
        if (*s1 == '\0')  return 0;
        s1++;
        s2++;
    }
    return (*s1 & 0xff) - (*s2 & 0xff);
}
/*
*********************************************************************************************************
--	
--
*********************************************************************************************************
*/
INT8U Strlen(char * buf)
{   
	INT8U i;

    for (i = 0;i < 255;i ++) 
	{
        if (buf[i] < 0x20) return i;
    }
    return 255;
}
/*
*********************************************************************************************************
--
--
*********************************************************************************************************
*/
BOOL ScanString(INT8U * InputLine, INT8U * LineIndex, LPSTR s,INT16U len)
{
    INT16U i;

    ScanDelimiters(InputLine, LineIndex, SCAN_COMMA);
    i = 0;
    while ((InputLine[*LineIndex] != ' ') && (InputLine[*LineIndex] !=  0 ) && (i <= len-1))
        s[i++] = tolow(InputLine[(*LineIndex)++]);

    s[i] = 0;

    if (i != 0) return TRUE;

    return FALSE;
}
/*
*********************************************************************************************************
--
--
*********************************************************************************************************
*/
BOOL GetInputLine(INT8U * InputLine, INT8U ch,BOOL echo,INT16U to)
{
    BOOL done ;
    BOOL gotline;
    INT8U err;
    INT8U newchar;
    INT8U LineIndex;
    INT16U MaxLen;
    LineIndex = 0;
    done = FALSE;
    gotline = FALSE;
    if (echo) MaxLen = MAX_LINE - 1 ;else MaxLen = 254;
    while (!done) 
	{
      newchar = CommGetChar(ch, to, &err)  & 0x7F;
      if(err != COMM_NO_ERR) return FALSE;
      if(newchar == 0x7f) return FALSE;
      switch (newchar) 
	  {
      case ASCII_CR:
      case ASCII_LF:
	  	  {
			InputLine[LineIndex] = 0;
			done = TRUE;
			gotline = TRUE;
			break;
		  }
      case ASCII_BS:
          {
  			if(LineIndex > 0) 
			{
    			LineIndex--;
    			if(echo) 
				{
                	CommPutChar(ch,ASCII_BS,0);
                	CommPutChar(ch,' ',0);
                	CommPutChar(ch,ASCII_BS,0);
				}
			}
			break;
		   }
      default:
	       {
			if(newchar == ASCII_TAB)   
				newchar = ' ';
	        if(newchar >= ' ') 
			{
	            if(LineIndex < MaxLen ) 
				{
	                InputLine[LineIndex] = newchar;
	                if (echo)   CommPutChar(ch,newchar,0);
	                LineIndex++;
	            } 
				else 
					CommPutChar(ch,ASCII_BELL,0);
	    } 
			else 
			;
	         break;
		   }
       }
    }
    return gotline;
}
/*
*********************************************************************************************************
--
--
*********************************************************************************************************
*/
void Print(INT8U ch, char *format,...)
{
    INT8U i,err,Len;
    COMM_RING_BUF *pbuf;
    va_list ap;
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr;
    cpu_sr = 0;                                  /* Prevent compiler warning                           */
#endif
    OS_ENTER_CRITICAL();
    switch (ch) 
	{
       case COM1:
            pbuf     = &Comm1Buf;                     		/* Initialize the ring buffer for COM1     */
            break;
       case COM2:
            pbuf     = &Comm2Buf;                     		/* Initialize the ring buffer for COM2     */
            break;
       case COM3:
            pbuf     = &Comm3Buf;                     		/* Initialize the ring buffer for COM3     */
            break;     
       case COM4:
            pbuf     = &Comm4Buf;                     		/* Initialize the ring buffer for COM4     */
            break;  
       case COM5:
            pbuf     = &Comm5Buf;                     		/* Initialize the ring buffer for COM5     */
            break;     
       case COM6:
            pbuf     = &Comm6Buf;                     		/* Initialize the ring buffer for COM6     */
            break;
#ifdef Peripherals
       case BSPI1:
            pbuf     = &SPI1Buf;                      		/* Initialize the ring buffer for SPI1     */
            break;
       case BSPI2:
            pbuf     = &SPI2Buf;                      		/* Initialize the ring buffer for SPI2     */
            break;
       case BSPI3:
            pbuf     = &SPI3Buf;                      		/* Initialize the ring buffer for SPI3     */
            break;
#endif
		default:
			 OS_EXIT_CRITICAL();
			 return;
    }
    OS_EXIT_CRITICAL();

    OSSemPend(pbuf->InPrintSem, 0 , &err);

    va_start(ap,format);
    Len = vsprintf((char *)pbuf->Pbuf,format,ap);
    va_end(ap);

    for (i =0;i < Len; i++) {
        CommPutChar(ch, pbuf->Pbuf[i],0);
    }
    OSSemPost(pbuf->InPrintSem);
}
/*
*********************************************************************************************************
--
--
*********************************************************************************************************
*/
void EM_CMD(INT8U ch)
{
    INT16U i;
    INT8U  err = 1;
    INT8U  InputLine[MAX_LINE+4];                                    /* Characters which have been input */
    INT8U  LineIndex;
    LPSTR  tmpStr;
    INT8U  tmp[20];                                                  /* Pointer to next character to scan */
    if (GetInputLine(InputLine, ch, TRUE,0)) 
	{
		LineIndex = 0;
		tmpStr = (LPSTR)tmp;
		ScanString(InputLine, &LineIndex, tmpStr,1);
		for (i = 0;i < CmdCnt;i++) 
		{
				LineIndex = 0;
				tmpStr = (LPSTR)&tmp[0];
				if (ScanString(InputLine, &LineIndex, tmpStr, Strlen((char *)cmd_list[i].cmd)+1)) 
				{
					if (strcmp1((char *)tmpStr,(char *)cmd_list[i].cmd)) 
						err = 1;
					else 
					{
					    err = 0;
					    break;
					}
				} else err = 1;
			}
			if (tmpStr[0] == 0)
				Print(ch, "\n\r>");
			else 
			{
			  if (err == 0 )
			  {
				  cmd_list[i].cmd_func(ch,&InputLine[LineIndex]);
				  memset(InputLine,0,MAX_LINE+4);
			  } 
			  else 
			  {
				  Print(ch, "\n\rError Command!\n\r>");
			  }
			}
    }
}
/*
*********************************************************************************************************
--  add acommand £¬success : return(0) and fail : return (-1).
*********************************************************************************************************
*/ 
int AddCmd(INT8U *cmd,void (*CmdFunc)(INT8U ch,INT8U * s),INT8U *help)
{
    if (cmd_list[CmdCnt].cmd[0] == '\0') {
        cmd_list[CmdCnt].cmd_func = CmdFunc;
        strcpy((char *)cmd_list[CmdCnt].cmd,(char *)cmd);
        strcpy((char *)cmd_list[CmdCnt].help,(char *)help);
        if (CmdCnt < MaxCmdCnt) {
          CmdCnt ++;
          return (0);
        }
    }
    return (-1);
}

 #endif
