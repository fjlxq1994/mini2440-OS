#include "vsprintf.h"


unsigned char outbuf[1024];
unsigned char inbuf[1024];

int printf(const char* fmt, ...)
{
		int i = 0;	//���ɳ�ʼ��������ϰ��
		va_list args;		
		
		//�����ת��Ϊ�ַ���
		va_start(args, fmt);
		
		vsprintf((char*)outbuf, fmt, args);
		
		va_end(args);
					
	
		//��ӡ�ַ�������
		for(i=0; i<strlen((char*)outbuf); i++) {
			
				send_byte(outbuf[i]);			
		}
		
		return i;
}


int scanf(const char* fmt, ...)
{
		int i = 0;	//���ɳ�ʼ��������ϰ��
		unsigned char c;
		va_list args;
		
		//��ȡ�����ַ���
		while(1) {
			
				c = rece_byte();	
				
				if( (c == 0x0d) || (c == 0x0a) ) {
					
						inbuf[i] = '\n';
						break;
				
				} else {
					
						inbuf[i++] = c;	
				}	
		}
		
		//��ʽת��	
		va_start(args, fmt);
		vsscanf((char*)inbuf, fmt, args);
		va_end(args);
		
		return i;
}

