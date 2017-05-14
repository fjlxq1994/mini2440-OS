/**
 * * 中断配置
 *   使能中断响应的钩子函数
 *   使能只需定义宏
 *   如 #define INT_ADC_EN 1u
 *
 */

#ifndef _INT_CFG_H
#define _INT_CFG_H

#define INT_TIMER4_EN 0u  /*打开timer4中断处理的钩子函数*/
#define EINT0_EN   0u
#define EINT1_EN   0u
#define EINT2_EN   0u
#define EINT3_EN   0u
#define EINT4_7_EN   0u
#define EINT8_23_EN   0u
#define INT_CAM_EN  0u
#define nBATT_FLT_EN 0u

#define INT_TICK_EN 0u
#define INT_WDT_AC97_EN 0u
#define INT_TIMER0_EN 1u
#define INT_TIMER1_EN 0u

#define INT_TIMER2_EN 0u
#define INT_TIMER3_EN 0u
#define INT_UART2_EN 0u

#define INT_LCD_EN 0u
#define INT_DMA0_EN 0u
#define INT_DMA1_EN 0u
#define INT_DMA2_EN 0u

#define INT_DMA3_EN 0u
#define INT_SDI_EN 0u
#define INT_SPI0_EN 0u
#define INT_UART1_EN 0u

#define INT_NFCON_EN 0u 
#define INT_USBD_EN 0u
#define INT_USBH_EN 0u
#define INT_IIC_EN 0u

#define INT_ADC_EN 0u 
#define INT_RTC_EN 0u
#define INT_SPI1_EN 0u
#define INT_UART0_EN 1u

#endif
