#ifndef _LED_H
#define _LED_H

#include "os_cpu.h"

#define rGPBCON  *((volatile unsigned long*)0x56000010)
#define rGPBDAT  *((volatile unsigned long*)0x56000014)
#define rGPBUP   *((volatile unsigned long*)0x56000018)

#define GPB5678_OUT ((1<<10)|(1<<12)|(1<<14)|(1<<16))
#define LED1  (1<<5)
#define LED2 (1<<6)
#define LED3 (1<<7)
#define LED4 (1<<8)

void led_init();
void Gpio_Led1(INT8U);
void Gpio_Led2(INT8U);
void Gpio_Led3(INT8U);
void Gpio_Led4(INT8U);

#endif
