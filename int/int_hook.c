
#ifndef _INT_HOOK_H
#define _INT_HOOK_H

#include "int_hook.h"
#include "interrupt.h"
#include "printf.h"
#include "led.h"

void int_adc_hook()
{

}

void int_rtc_hook()
{

}

void int_spi1_hook()
{

}

void int_uart0_hook()
{

}

void int_iic_hook()
{

}

void int_usbh_hook()
{

}

void int_usbd_hook()
{

}

void int_nfcon_hook()
{

}

void int_uart1_hook()
{

}

void int_spi0_hook()
{

}

void int_sdi_hook()
{

}

void int_dma3_hook()
{

}

void int_dma2_hook()
{

}

void int_dma1_hook()
{

}

void int_dma0_hook()
{

}

void int_lcd_hook()
{

}

void int_uart2_hook()
{

}

#define INT_TIMER4_BIT 14 
void int_timer4_hook()
{
    OSTimeTick();//uCOS的时钟节拍函数
    //Gpio_Led2(0);
    rSRCPND |= (0x1 << INT_TIMER4_BIT);
    rINTPND |= (0x1 << INT_TIMER4_BIT); 
    //rSRCPND = (0x1 << INT_TIMER4_BIT);
    //rINTPND = rINTPND;   
    //
}

void int_timer3_hook()
{
    printf("\n\r Timer3 Hook \n\r");
}

void int_timer2_hook()
{

}

void int_timer1_hook()
{

}

void int_timer0_hook()
{
	OSTimeTick();
	rSRCPND |=( 1 << 10);
	rINTPND |= ( 1 << 10);
}

void int_wdt_ac97_hook()
{

}

void int_tick_hook()
{

}

void int_nBATT_FLT_hook()
{

}

void int_cam_hook()
{

}

void eint8_23_hook()
{

}

void eint4_7_hook()
{

}

void eint0_hook()
{

}

void eint1_hook()
{

}

void eint2_hook()
{

}

void eint3_hook()
{

}




#endif
