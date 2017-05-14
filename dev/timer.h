#ifndef _TIMER_H
#define _TIMER_H

#define rTCFG0 (*(volatile unsigned int *)0x51000000)
#define rTCFG1 (*(volatile unsigned int *)0x51000004)
#define rTCON (*(volatile unsigned int *)0x51000008)
#define rTCNTB0 (*(volatile unsigned int *)0x5100000C)
#define rTCMPB0 (*(volatile unsigned int *)0x51000010)
#define rTCNTO0 (*(volatile unsigned int *)0x51000014)
#define rTCNTB1 (*(volatile unsigned int *)0x51000018)
#define rTCMPB1 (*(volatile unsigned int *)0x5100001C)
#define rTCNTO1 (*(volatile unsigned int *)0x51000020)
#define rTCNTB2 (*(volatile unsigned int *)0x51000024)
#define rTCMPB2 (*(volatile unsigned int *)0x51000028)
#define rTCNTO2 (*(volatile unsigned int *)0x5100002C)
#define rTCNTB3 (*(volatile unsigned int *)0x51000030)
#define rTCMPB3 (*(volatile unsigned int *)0x51000034)
#define rTCNTO3 (*(volatile unsigned int *)0x51000038)
#define rTCNTB4 (*(volatile unsigned int *)0x5100003C)
#define rTCNTO4 (*(volatile unsigned int *)0x51000040)


void timer4_start();
void timer4_stop();
void timer4_init();

void Timer0_Handler();
void Timer0Init();

#endif
