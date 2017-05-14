//�˿ڿ��ƼĴ���
#define rGPCCON (*(volatile unsigned long *)0x56000020)
#define rGPDCON (*(volatile unsigned long *)0x56000030)
#define rGPGCON (*(volatile unsigned long *)0x56000060)

//LCD���ƼĴ���
#define	 	rLCDCON1 			(*(volatile unsigned long *)0x4D000000)
#define 	rLCDCON2 			(*(volatile unsigned long *)0x4D000004)
#define 	rLCDCON3 			(*(volatile unsigned long *)0x4D000008)
#define		rLCDCON4		  (*(volatile unsigned long *)0x4D00000C)
#define 	rLCDCON5 			(*(volatile unsigned long *)0x4D000010)

//STN/TFT: ֡(frame)��������ʼ��ַ�Ĵ���
#define 	rLCDSADDR1 		(*(volatile unsigned long *)0x4D000014)
#define 	rLCDSADDR2 		(*(volatile unsigned long *)0x4D000018)

//STN/TFT��������Ļ��ַ���üĴ���
#define 	rLCDSADDR3 		(*(volatile unsigned long *)0x4D00001C)

//TFT ����ʱ��ɫ��Ĵ���
#define 	rTPAL 				(*(volatile unsigned long *)0x4D000050)

//VCLK������Ļ�ֲ��е�DCLK
#define	 	VSPW 2					//ͨ��������Ч������ֱͬ�������Ⱦ���VSYNC ����ĸߵ�ƽ���
#define 	VBPD 9					//��ֱͬ�����ں����Ч����
#define 	LINEVAL 239			//LCD���Ĵ�ֱ�ߴ�
#define 	VFPD 1					//��ֱǰ��Ϊ֡����ʱ����ֱͬ������ǰ�ĵ���Ч����
#define 	CLKVAL 6   			//VCLK = HCLK / [(CLKVAL  + 1)   �� 2]  
#define 	HSPW 0x01				//ͨ������VCLK����ˮƽͬ�������Ⱦ���HSYNC ����ĸߵ�ƽ��� 
#define 	HBPD 0x05				//����ˮƽ����ΪHSYNC ���½�������Ч���ݵĿ�ʼ֮���VCLK �������� 
#define 	HOZVAL 319			//LCD����ˮƽ�ߴ�
#define 	HFPD 0x0c				//ˮƽ����Ϊ��Ч���ݵĽ�����HSYNC ��������֮���VCLK �������� 

unsigned short LCDBUFFER[240][320];  //LCD֡�������� 16bit����

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
    
    // 5:6:5 ��ʽ  VLINE/HSYNC ���弫�ԡ�VFRAME/VSYNC ���弫�Է�ת(LCD�ͺž���)
    rLCDCON5 = ((1<<11) | (1 << 9) | (1 << 8) | (1 << 0));
    
    //LCDSADDR1 ��Ϊ LCDBANK[29:21]��LCDBASEU[20:0]������
    //LCDBANK[29:21]����֡������ʼ��ַ��[30:22]
    //LCDBASEU[20:0]����֡������ʼ��ַ��[21:1]
    rLCDSADDR1 = (((U32)LCDBUFFER>>22)<<21) | (((U32)LCDBUFFER>>1)&0x1fffff);
    
    //���֡������ַ[21:1]  
    rLCDSADDR2 = (((U32)LCDBUFFER+240*320*2)>>1)&0x1fffff;
    
    //LCDSADDR3 ��Ϊ OFFSIZE[21:11]��PAGEWIDTH[10:0]������
    //OFFSIZE[21:11]���ʾ������ƫ�Ƴߴ� 
    //����һ��������ص㵽��һ�е�һ�����ص�֮�������ٸ����ص�  
    //PAGEWIDTH[10:0]���ʾ�еĿ�ȣ�����Ϊ��λ 16bit��  
    rLCDSADDR3 = (0<<11) | (320*2/2);
    
    rTPAL = 0;		//�ر���ʱ��ɫ��
}


void lcd_init()
{
    lcd_port_init();
    lcd_control_init();
    
    //��LCD��Դ
    rGPGCON |= (0x3<<8);	//����GPG4����ΪLCD_PWRDNģʽ 
    rLCDCON5 |= (1<<3);   //LCD_PWREN����ź�ʹ��
    rLCDCON1 |= 1;				// ������Ƶ����� LCD �����ź�
}

/* ���㺯�� */
void point(unsigned int x,unsigned int y,unsigned int color)
{
    unsigned int red, green, blue;  //LCDCON5������Ϊ5:6:5 ��ʽ r:g:b 
    
    red = (color>>19) & 0x1f;
    green = (color>>10) & 0x3f;
    blue = (color>>3) & 0x1f;
    //�����ص�װ�ص�LCD֡����������
    LCDBUFFER[y][x] = (unsigned short)((red<<11)|(green<<5)|blue);	
}

/* ���ߺ��� y0�� */
void Paint_Line(unsigned int y0,unsigned int color)
{
    unsigned int x;
    
    for(x=0; x<320; x++) 						
        point(x++,y0,color);  
}



/* ��ͼ���� ��㣨x0,y0��*/
void Paint_Bmp(U16 x0,U16 y0,U16 wide,U16 high,const U8 *bmp)
{
    U16 x,y;
    U16 c;
    U32 p = 0;
    
    for( y = y0 ; y < y0+high ; y++ )
    {
    	for( x = x0 ; x < x0+wide ; x++ )
    	{
    	    c = bmp[p] | (bmp[p+1]<<8) ;	//bmp.c�е�����Ԫ�ش�С��8bit����Ļ��������16bit									
	    
	    if ( ( x < 320) && ( y < 240) )   //LCD�ߴ緶Χ  ����ʵ��LCD����
	        LCDBUFFER[y][x] = c ;
		
	    p = p + 2 ;	  //��Ļ��������16bit,����Ҫ+2
    	}
    }
}

/* ��ʱ��ɫ�� */
void clearSrc(unsigned int color)
{
    rTPAL = (1<<24)|(color&0xffffff);	//ʹ����ʱ���԰�������Ϊ��ɫ
}

extern unsigned char bmp[64240];  //�����С����bmp.c��С��������
																	//����ʱ������ͼ��ͷ����

void lcd_test()
{  
  	//Paint_Line(120,0xff0000);
        
   //200,200����bmp.c��������ͼ��:(xxx,xxx)������
   //����bmp.cʱ��ɨ��ģʽ���ֶ��п����ǣ��ó���
   //16λ���ɫ,������ͼ��ͷ����
    Paint_Bmp(0,0,200,200,bmp);		
    
   // clearSrc(0xff0000);
}



//��ɫ�� http://www.114la.com/other/rgb.htm


