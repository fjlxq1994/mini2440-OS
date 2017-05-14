#ifndef COMM_H
#define COMM_H
#include "os_cpu.h"
#include "ucos_ii.h"
#include "interrupt.h"
#include "vsprintf.h"
#include <stdlib.h>
#define  SHELL_ENABLE           1


#define  COM1                   1
#define  COM2                   2
#define  COM3                   3
#define  COM4                   4
#define  COM5                   5
#define  COM6                   6


#define  BSPI1                  7
#define  BSPI2                  8
#define  BSPI3                  9

#define  BI2C2                   10

#define  COMM_RX_BUF_SIZE       1024               /* Number of characters in Rx ring buffer             */
#define  COMM_TX_BUF_SIZE       1024               /* Number of characters in Tx ring buffer             */

#ifndef  NUL
#define  NUL                    0x00
#endif
                                                 /* ERROR CODES                                        */
#define  COMM_NO_ERR            0                /* Function call was successful                       */
#define  COMM_BAD_CH            1                /* Invalid communications port channel                */
#define  COMM_RX_EMPTY          2                /* Rx buffer is empty, no character available         */
#define  COMM_TX_FULL           3                /* Tx buffer is full, could not deposit character     */
#define  COMM_TX_EMPTY          4                /* If the Tx buffer is empty.                         */
#define  COMM_RX_TIMEOUT        5                /* If a timeout occurred while waiting for a character*/
#define  COMM_TX_TIMEOUT        6                /* If a timeout occurred while waiting to send a char.*/
#define  COMM_RD_END_TIMEOUT    7
#define  COMM_RD_CHAR_TIMEOUT   8

typedef enum
{
	RECEIVE,SEND
}IRDA_STATUS_TYPE;

typedef struct
{
	volatile IRDA_STATUS_TYPE status;                      //发送还是接受
	volatile unsigned char bit_cnt;						  //发送或接受到多少位
	volatile unsigned char c;							  //发送或接受的数据
	volatile unsigned char check;                         //校验位
}IRDA_COMM_TYPE;


typedef struct
{
    volatile INT16U     RingBufRxCtr;                /* Number of characters in the Rx ring buffer              */
    OS_EVENT  *RingBufRxSem;                /* Pointer to Rx semaphore                                 */
    INT8U     *RingBufRxInPtr;              /* Pointer to where next character will be inserted        */
    INT8U     *RingBufRxOutPtr;             /* Pointer from where next character will be extracted     */
    INT8U      RingBufRx[COMM_RX_BUF_SIZE]; /* Ring buffer character storage (Rx)                      */
    volatile INT16U     RingBufTxCtr;                /* Number of characters in the Tx ring buffer              */
    OS_EVENT  *RingBufTxSem;                /* Pointer to Tx semaphore                                 */
    INT8U     *RingBufTxInPtr;              /* Pointer to where next character will be inserted        */
    INT8U     *RingBufTxOutPtr;             /* Pointer from where next character will be extracted     */
    INT8U      RingBufTx[COMM_TX_BUF_SIZE]; /* Ring buffer character storage (Tx)                      */
    OS_EVENT  *InPrintSem;
    INT8U      Pbuf[COMM_TX_BUF_SIZE];
}COMM_RING_BUF;


#define I2C_REC	 0x99
#define I2C_OK	 0xaa

typedef struct
{ 
	volatile unsigned char status;
	volatile unsigned short len;
	volatile unsigned char *p;
}I2C_CTR_TYPE;
#ifdef SHELL_ENABLE	
typedef struct 
{
  INT8U cmd[8];
  INT8U help[20];
  void (*cmd_func)(INT8U ch,INT8U *s);
} cmd_list_type;

typedef INT16U BOOL;

typedef INT32U   		*    LPINT32U;
typedef INT16U   		*    LPINT16U;
typedef INT8U    		*    LPINT8U;
typedef BOOL     		*    LPBOOL;
typedef INT8S     	*    LPSTR;
typedef const INT8S *    LPCSTR;

typedef void   *    PVOID;
typedef INT32U *    PINT32U;
typedef INT16U *    PINT16U;
typedef INT8U  *    PINT8U;
typedef BOOL   *    PBOOL;

#define  ASCII_CR    0x0d
#define  ASCII_LF    0x0a
#define  ASCII_BS    0x08
#define  ASCII_BELL  0x07
#define  ASCII_TAB   0x09
#define  ASCII_XON   0x11
#define  ASCII_XOFF  0x13
#define  ASCII_ESC   0X1b

#define  SCAN_COMMA     1              /* Use comma as a delimiter(,)       */
#define  SCAN_EQUALS    2              /* Use equals as a delimiter(=)      */
#define  SCAN_DOT       3              /* Use dot as a delimiter(.)         */

#define  MAX_LINE    32              	 /* Size of input line */
#define  MaxCmdCnt   10

extern 	INT16U CmdCnt;
extern 	cmd_list_type cmd_list[MaxCmdCnt];
extern  IRDA_COMM_TYPE irda_ctr;
void 		EM_CMD(INT8U ch);
int 		AddCmd(INT8U *cmd,void (*CmdFunc)(INT8U ch,INT8U * s),INT8U *help);
void 		Print(INT8U ch, char *format,...);

#endif
void   CommInit (INT8U ch);
void   CommPutRxChar (INT8U ch, INT8U c);

INT8U  CommGetChar (INT8U ch, INT16U to, INT8U *err);
INT8U  CommPutChar (INT8U ch, INT8U c, INT16U to);
//INT8U  CommCfgPort (INT8U ch, INT32U baud, short parity,short bits,short wide);
INT8U  CommGetTxChar (INT8U ch, INT8U *err);
unsigned char  query_txbuf_empty(INT8U ch);
unsigned char  query_rxbuf_empty(INT8U ch);
//void irda_send_enable(void);
//void irda_receive_enable(void);
//void irda_receive_interrupt_enable(void);

//extern I2C_CTR_TYPE   i2c2ctr;
#endif
