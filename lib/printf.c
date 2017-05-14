#include "vsprintf.h"


unsigned char outbuf[1024];
unsigned char inbuf[1024];

int printf(const char* fmt, ...)
{
		int i = 0;	//养成初始化变量的习惯
		va_list args;		
		
		//将变参转化为字符串
		va_start(args, fmt);
		
		vsprintf((char*)outbuf, fmt, args);
		
		va_end(args);
					
	
		//打印字符到串口
		for(i=0; i<strlen((char*)outbuf); i++) {
			
				send_byte(outbuf[i]);			
		}
		
		return i;
}


int scanf(const char* fmt, ...)
{
		int i = 0;	//养成初始化变量的习惯
		unsigned char c;
		va_list args;
		
		//获取输入字符串
		while(1) {
			
				c = rece_byte();	
				
				if( (c == 0x0d) || (c == 0x0a) ) {
					
						inbuf[i] = '\n';
						break;
				
				} else {
					
						inbuf[i++] = c;	
				}	
		}
		
		//格式转化	
		va_start(args, fmt);
		vsscanf((char*)inbuf, fmt, args);
		va_end(args);
		
		return i;
}

