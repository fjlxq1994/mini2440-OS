/**
 *  中断的一些钩子函数
 *  在处理中断时如果配置了函数钩子之后就会在处理时被调度
 * 
 *  在需要调用使能钩子函数时只需在int_conf.h中对相应的宏进行定义
 *  且要实现该文件中的钩子函数
 *  钩子函数最后还要清除中断
 */

#ifndef _INT_HOOK_H
#define _INT_HOOK_H

void int_adc_hook();
void int_rtc_hook();
void int_spi1_hook();
void int_uart0_hook();
void int_iic_hook();
void int_usbh_hook();
void int_usbd_hook();
void int_nfcon_hook();
void int_uart1_hook();
void int_spi0_hook();
void int_sdi_hook();
void int_dma3_hook();
void int_dma2_hook();
void int_dma1_hook();
void int_dma0_hook();
void int_lcd_hook();
void int_uart2_hook();
void int_timer4_hook();
void int_timer3_hook();
void int_timer2_hook();
void int_timer1_hook();
void int_timer0_hook();
void int_wdt_ac97_hook();
void int_tick_hook();
void int_nBATT_FLT_hook();
void int_cam_hook();
void eint8_23_hook();
void eint4_7_hook();
void eint0_hook();
void eint1_hook();
void eint2_hook();
void eint3_hook();




#endif
