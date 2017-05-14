#ifndef _UART_LIB_H

#define _UART_LIB_H

void Uart_SendString(char *pt);
void Uart_SendByte(int data);

void Uart_Printf(char *fmt,...);
#endif