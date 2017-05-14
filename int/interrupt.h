#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "os_cpu.h"
#define rINTMSK (*(volatile unsigned long*)0x4A000008)
#define rSRCPND (*(volatile unsigned long*)0x4A000000)
#define rINTMOD (*(volatile unsigned long*)0x4A000004)
#define rPRIORITY (*(volatile unsigned long*)0x4A00000C)
#define rINTPND (*(volatile unsigned long*)0x4A000010)
#define rINTOFFSET (*(volatile unsigned long*)0x4A000014) /*Read Only*/
#define rSUBSRCPND (*(volatile unsigned long*)0x4A000018)
#define rINTSUBMSK (*(volatile unsigned long*)0x4A00001C)

#define BIT_SUB_TXD1	(0x1<<4)
#define BIT_SUB_RXD1	(0x1<<3)
#define BIT_SUB_ERR0	(0x1<<2)
#define BIT_SUB_TXD0	(0x1<<1)
#define BIT_SUB_RXD0	(0x1<<0)

void disable_int();
void enable_int();
void handle_int();
void change_int_mode(INT8U);
void change_int_priority(INT8U,INT8U,BOOLEAN);
void int_mask(INT8U);
void enable_certain_int(INT8U);

__inline void ClearPending(int bit);

__inline void ClearSubPending(int bit);          

#define	EnableIrq(bit)		rINTMSK &= ~(bit)
#define	DisableIrq(bit)		rINTMSK |= (bit)
#define	EnableSubIrq(bit)	rINTSUBMSK &= ~(bit)
#define	DisableSubIrq(bit)	rINTSUBMSK |= (bit)
#endif



