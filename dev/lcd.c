//端口控制寄存器
#define rGPCCON (*(volatile unsigned long *)0x56000020)
#define rGPDCON (*(volatile unsigned long *)0x56000030)
#define rGPGCON (*(volatile unsigned long *)0x56000060)

//LCD控制寄存器
#define	 	rLCDCON1 			(*(volatile unsigned long *)0x4D000000)
#define 	rLCDCON2 			(*(volatile unsigned long *)0x4D000004)
#define 	rLCDCON3 			(*(volatile unsigned long *)0x4D000008)
#define		rLCDCON4		  (*(volatile unsigned long *)0x4D00000C)
#define 	rLCDCON5 			(*(volatile unsigned long *)0x4D000010)

//STN/TFT: 帧(frame)缓冲区开始地址寄存器
#define 	rLCDSADDR1 		(*(volatile unsigned long *)0x4D000014)
#define 	rLCDSADDR2 		(*(volatile unsigned long *)0x4D000018)

//STN/TFT：虚拟屏幕地址设置寄存器
#define 	rLCDSADDR3 		(*(volatile unsigned long *)0x4D00001C)

//TFT ：临时调色板寄存器
#define 	rTPAL 				(*(volatile unsigned long *)0x4D000050)

//VCLK就是屏幕手册中的DCLK
#define	 	VSPW 2					//通过计算无效行数垂直同步脉冲宽度决定VSYNC 脉冲的高电平宽度
#define 	VBPD 9					//垂直同步周期后的无效行数
#define 	LINEVAL 239			//LCD面板的垂直尺寸
#define 	VFPD 1					//垂直前沿为帧结束时，垂直同步周期前的的无效行数
#define 	CLKVAL 6   			//VCLK = HCLK / [(CLKVAL  + 1)   × 2]  
#define 	HSPW 0x01				//通过计算VCLK的数水平同步脉冲宽度决定HSYNC 脉冲的高电平宽度 
#define 	HBPD 0x05				//描述水平后沿为HSYNC 的下降沿与有效数据的开始之间的VCLK 周期数。 
#define 	HOZVAL 319			//LCD面板的水平尺寸
#define 	HFPD 0x0c				//水平后沿为有效数据的结束与HSYNC 的上升沿之间的VCLK 周期数。 

unsigned short LCDBUFFER[240][320];  //LCD帧缓冲数组 16bit数组

typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;

void lcd_port_init()
{
    rGPDCON = 0xaaaaaaaa;
    rGPCCON = 0xaaaaaaaa;		
}

void lcd_control_init()
{	
    rLCDCON1 = (CLKVAL<<8)|(0x3<<5)|(0xc<<1)|(0<<0);  //16 bpp for TFT
    rLCDCON2 = (VBPD<<24)|(LINEVAL<<14)|(VFPD<<6)|(VSPW);
    rLCDCON3 = (HBPD<<19)|(HOZVAL<<8)|(HFPD);
    rLCDCON4 = (HSPW);
    
    // 5:6:5 格式  VLINE/HSYNC 脉冲极性、VFRAME/VSYNC 脉冲极性反转(LCD型号决定)
    rLCDCON5 = ((1<<11) | (1 << 9) | (1 << 8) | (1 << 0));
    
    //LCDSADDR1 分为 LCDBANK[29:21]，LCDBASEU[20:0]两个域
    //LCDBANK[29:21]域存放帧缓冲起始地址的[30:22]
    //LCDBASEU[20:0]域存放帧缓冲起始地址的[21:1]
    rLCDSADDR1 = (((U32)LCDBUFFER>>22)<<21) | (((U32)LCDBUFFER>>1)&0x1fffff);
    
    //存放帧结束地址[21:1]  
    rLCDSADDR2 = (((U32)LCDBUFFER+240*320*2)>>1)&0x1fffff;
    
    //LCDSADDR3 分为 OFFSIZE[21:11]，PAGEWIDTH[10:0]两个域
    //OFFSIZE[21:11]域表示虚拟屏偏移尺寸 
    //即上一行最后像素点到下一行第一个像素点之间间隔多少个像素点  
    //PAGEWIDTH[10:0]域表示行的宽度（半字为单位 16bit）  
    rLCDSADDR3 = (0<<11) | (320*2/2);
    
    rTPAL = 0;		//关闭临时调色板
}


void lcd_init()
{
    lcd_port_init();
    lcd_control_init();
    
    //打开LCD电源
    rGPGCON |= (0x3<<8);	//设置GPG4引脚为LCD_PWRDN模式 
    rLCDCON5 |= (1<<3);   //LCD_PWREN输出信号使能
    rLCDCON1 |= 1;				// 允许视频输出和 LCD 控制信号
}

/* 画点函数 */
void point(unsigned int x,unsigned int y,unsigned int color)
{
    unsigned int red, green, blue;  //LCDCON5中设置为5:6:5 格式 r:g:b 
    
    red = (color>>19) & 0x1f;
    green = (color>>10) & 0x3f;
    blue = (color>>3) & 0x1f;
    //把像素点装载到LCD帧缓冲数组中
    LCDBUFFER[y][x] = (unsigned short)((red<<11)|(green<<5)|blue);	
}

/* 画线函数 y0行 */
void Paint_Line(unsigned int y0,unsigned int color)
{
    unsigned int x;
    
    for(x=0; x<320; x++) 						
        point(x++,y0,color);  
}



/* 画图函数 起点（x0,y0）*/
void Paint_Bmp(U16 x0,U16 y0,U16 wide,U16 high,const U8 *bmp)
{
    U16 x,y;
    U16 c;
    U32 p = 0;
    
    for( y = y0 ; y < y0+high ; y++ )
    {
    	for( x = x0 ; x < x0+wide ; x++ )
    	{
    	    c = bmp[p] | (bmp[p+1]<<8) ;	//bmp.c中的数组元素大小是8bit，屏幕像素设置16bit									
	    
	    if ( ( x < 320) && ( y < 240) )   //LCD尺寸范围  根据实际LCD设置
	        LCDBUFFER[y][x] = c ;
		
	    p = p + 2 ;	  //屏幕像素设置16bit,所以要+2
    	}
    }
}

/* 临时调色板 */
void clearSrc(unsigned int color)
{
    rTPAL = (1<<24)|(color&0xffffff);	//使能临时调试板且设置为红色
}

extern unsigned char bmp[64240];  //数组大小根据bmp.c大小而决定的
																	//制作时不包含图像头数据

void lcd_test()
{  
  	//Paint_Line(120,0xff0000);
        
   //200,200制作bmp.c软件的输出图像:(xxx,xxx)决定的
   //制作bmp.c时，扫描模式两种都有可能是，得尝试
   //16位真彩色,不包含图像头数据
    Paint_Bmp(0,0,200,200,bmp);		
    
   // clearSrc(0xff0000);
}



//颜色表 http://www.114la.com/other/rgb.htm


