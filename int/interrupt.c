
/***
 * 关闭所有中断
 * 直接往中断屏蔽寄存器中写1即可
 */
#include "interrupt.h"
#include "int_cfg.h"
#include "int_hook.h"
#include "os_cpu.h"
#include "uart.h"
#include "led.h"

void disable_int()
{
    int mask = ~(0x0);
    rINTSUBMSK = mask;
    rINTMSK = mask;
}


void enable_int()
{
   rINTSUBMSK = 0x0;
   rINTMSK = 0x0;
   return 0;
}

/**
 * int的总处理方法
 * 也可以理解为中断的分配器dispatcher
 * 在start.S中如果发生了中断
 * 就调用这个方法
 * */
void handle_int()
{
   //根据int_pnd寄存器中的值来进行相应的中断处理
   //处理时调用相应的钩子函数

   int int_pnd = rINTPND;
  
#if (EINT0_EN > 0u)
   if(int_pnd & 0x1)
       eint0_hook();
#endif

#if (EINT1_EN > 0u)
   if(int_pnd & 0x2)
       eint1_hook();
#endif
   
#if (EINT2_EN > 0u)
   if(int_pnd & 0x4)
       eint2_hook();
#endif

#if (EINT3_EN > 0u)
   if(int_pnd & 0x8)
       eint3_hook();
#endif

#if (EINT4_7_EN > 0u)
   if(int_pnd & 0x10)
       eint4_7_hook();
#endif

#if (EINT8_23_EN > 0u)
   if(int_pnd & 0x20)
       eint8_23_hook();
#endif

#if (INT_CAM_EN > 0u)
   if(int_pnd & 0x40)
       int_cam_hook();
#endif

#if (nBATT_FLT_EN > 0u)
   if(int_pnd & 0x80)
       int_nBATT_FLT_hook();
#endif

#if (INT_TICK_EN > 0u)
   if(int_pnd & 0x100)
       int_tick_hook();
#endif

#if (INT_WDT_AC97_EN > 0u)
   if(int_pnd & 0x200)
       int_wdt_ac97_hook();
#endif

#if (INT_TIMER0_EN > 0u)
   if(int_pnd & 0x400)
       int_timer0_hook();
#endif

#if (INT_TIMER1_EN > 0u)
   if(int_pnd & 0x800)
       int_timer1_hook();
#endif

#if (INT_TIMER2_EN > 0u)
   if(int_pnd & 0x1000)
       int_timer2_hook();
#endif

#if (INT_TIMER3_EN > 0u)
   if(int_pnd & 0x2000)
       int_timer3_hook();
#endif

#if (INT_TIMER4_EN > 0u)
   if(int_pnd & 0x4000)
       int_timer4_hook();
#endif

#if (INT_UART2_EN > 0u)
   if(int_pnd & 0x8000)
       int_uart2_hook();
#endif

#if (INT_LCD_EN > 0u)
   if(int_pnd & 0x10000)
       int_lcd_hook();
#endif

#if (INT_DMA0_EN > 0u)
   if(int_pnd & 0x20000)
       int_dma0_hook();
#endif

#if (INT_DMA1_EN > 0u)
   if(int_pnd & 0x40000)
       int_dma1_hook();
#endif

#if (INT_DMA2_EN > 0u)
   if(int_pnd & 0x80000)
       int_dma2_hook();
#endif

#if (INT_DMA3_EN > 0u)
   if(int_pnd & 0x100000)
       int_dma3_hook();
#endif

#if (INT_SDI_EN > 0u)
   if(int_pnd & 0x200000)
       int_sdi_hook();
#endif

#if (INT_SPI0_EN > 0u)
   if(int_pnd & 0x400000)
       int_spi0_hook();
#endif

#if (INT_UART1_EN > 0u)
   if(int_pnd & 0x800000)
       int_uart1_hook();
#endif

#if (INT_NFCON_EN > 0u)
   if(int_pnd & 0x1000000)
       int_nfcon_hook();
#endif

#if (INT_USBD_EN > 0u)
   if(int_pnd & 0x2000000)
       int_usbd_hook();
#endif

#if (INT_USBH_EN > 0u)
   if(int_pnd & 0x4000000)
       int_usbh_hook();
#endif

#if (INT_IIC_EN > 0u)
   if(int_pnd & 0x8000000)
       int_iic_hook();
#endif

#if (INT_ADC_EN > 0u)
   if(int_pnd & 0x80000000)
       int_adc_hook();
#endif

#if (INT_RTC_EN > 0u)
   if(int_pnd & 0x40000000)
       int_rtc_hook();
#endif

#if (INT_SPI1_EN > 0u)
   if(int_pnd & 0x20000000)
       int_spi1_hook();
#endif

#if (INT_UART0_EN > 0u)
   if(int_pnd & 0x10000000)
       uart0_isr();
#endif
}

//修改特定的中断模式
//如果原中断为IRQ则转为FIQ
//如果原中断为FIQ则转为IRQ
//int_bit表示转变的中断所在的中断位
void change_int_mode(INT8U int_bit)
{
    if(int_bit > 31 || int_bit < 0)//合法性检测
	return;
    int int_mode = rINTMOD; //读回中断模式寄存器
    if((int_mode >> int_bit) & 0x01) /*对应位为1,则原相应中断的模式为快速中断*/
	int_mode &= (~(0x01 << int_bit));
    else //原中断为IRQ
	int_mode |= (~(0x01 << int_bit));
    rINTMOD = int_mode;
}

//改变中断的优先级次序
//APB_SEL : 中断的所在组
//order : 中断次序
//is_rotate : 是否优先级轮转
void change_int_priority(INT8U APB_SEL,INT8U order,BOOLEAN is_rotate)
{
    if(APB_SEL > 6 || order > 3 || order < 0 || is_rotate > 1 || is_rotate < 0)
	return;
    int priority = rPRIORITY;
    priority |= (order << (APB_SEL*2 + 7));
    priority |= (is_rotate << APB_SEL);
    rPRIORITY = priority;
}

void int_mask(INT8U int_bit)
{
   if(int_bit > 31 || int_bit < 0)
	return;
   int intmask = rINTMSK;
   intmask |= (0x01 << int_bit);
   rINTMSK = intmask;
}

//开启特定的中断
void enable_certain_int(INT8U int_bit)
{
   if(int_bit > 31 || int_bit <0)
       return;
   int intmask = rINTMSK;
   intmask &= (~(0x1<<int_bit));
   rINTMSK = intmask;
}

__inline void ClearPending(int bit)
{
//  int i;
  rSRCPND = bit;
  rINTPND = bit;
//  i = rINTPND;
}

__inline void ClearSubPending(int bit)
{
//  int i;
  rSUBSRCPND = bit; 
//  i = rINTPND;
}   

