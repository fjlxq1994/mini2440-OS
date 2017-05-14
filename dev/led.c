#include "os_cpu.h"
#include "led.h"
//led驱动程序
//led_init() GPIO_led1(ON/OFF) ...
//led1 led2 led3 led4 GPB5 GPB6 GBP7 GPB8


//led初始化
//将GPIOB的四个引脚设置为输出状态

void led_init()
{
    rGPBCON |= GPB5678_OUT; 
}

void Gpio_Led1(INT8U on)
{
    if(on)
	rGPBDAT &= ~(0x1 << 5);
    else rGPBDAT |= (0x1 << 5); 
	
}

void Gpio_Led2(INT8U on)
{
    if(on)
        rGPBDAT &= ~(0x1 << 6);
    else rGPBDAT |= (0x1 << 6);

}

void Gpio_Led3(INT8U on)
{
    if(on)
        rGPBDAT &= ~(0x1 << 7);
    else rGPBDAT |= (0x1 << 7);

}

void Gpio_Led4(INT8U on)
{
    if(on)
        rGPBDAT &= ~(0x1 << 8);
    else rGPBDAT |= (0x1 << 8);

}

