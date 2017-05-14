// 蜂鸣器
// GPB0
// 与LED在同一组GPIO 操作时要注意
// 不能影响LED

#include "beep.h"

#define rGPBCON   (*(volatile unsigned int *)0x56000010)
#define rGPBDAT  (*(volatile unsigned int *)0x56000014)
static beepStatus = 0;
// 蜂鸣器初始化
// 将GPB0设置为输出模式
void beep_init(){
	rGPBCON |= (1<<0);
}

// 蜂鸣器闪鸣
void beep_falsh(){
	rGPBDAT |= (beepStatus<<0);
	beepStatus = ~beepStatus;
}