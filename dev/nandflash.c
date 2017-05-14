#define rNFCONF (*(volatile unsigned long*)0x4e000000)	//NAND Flash ���üĴ��� 
#define rNFCONT (*(volatile unsigned long*)0x4e000004)	//NAND Flash ���ƼĴ��� 
#define rNFCMD  (*(volatile unsigned char*)0x4e000008)	//NAND Flash ����Ĵ���
#define rNFDATA (*(volatile unsigned char*)0x4e000010)	//NAND Flash ���ݼĴ��� 
#define rNFADDR (*(volatile unsigned char*)0x4e00000C)	//NAND Flash ��ַ���Ĵ���
#define rNFSTAT (*(volatile unsigned char*)0x4e000020)	//NAND Flash ����״̬�Ĵ���

//HCLK 100M  10ns
#define		TACLS			1		//CLE ��ALE ����ֵ����  >0ns  Duration = HCLK �� TACLS  
#define		TWRPH0		2		//TWRPH0����ֵ   Duration(>12ns) = HCLK �� ( TWRPH0 + 1 ) 
#define		TWRPH1		1		//TWRPH1����ֵ   Duration(>5ns ) = HCLK �� ( TWRPH1 + 1 ) 


void select_chip()
{
		rNFCONT &= ~(1<<1);	  //ʹ��Ƭѡ
}

void deselect_chip()
{
		rNFCONT |= (1<<1);		//��ֹƬѡ
}

void clear_RnB()
{
		rNFSTAT |= (1<<2); 		//д1���RnBλ
}

void send_cmd(unsigned cmd)  //cmd 8bit
{	
		rNFCMD = cmd;					//��������
}

void send_addr(unsigned addr)	 //addr 8bit
{
		rNFADDR = addr;				//���յ�ַ
}

void wait_RnB()  //��nandflash�ṩʱ���ǰ������ø��²���Ч
{
			while( !(rNFSTAT & (1<<2)) );	 
}


void nand_reset()
{
		//ѡ��flash
		select_chip();
		
		//���RnB
		clear_RnB();
		
		//����0xff����
		send_cmd(0xff);
		
		//�ȴ�RnB		
		wait_RnB();
		
		//ȡ��ѡ��flash	
		deselect_chip();
}


void nand_init()
{
		//��ʼ��NFCONF
		rNFCONF = (TWRPH1<<4) | (TWRPH0<<8) | (TACLS<<12);
		
		//��ʼ��NFCONT
		rNFCONT = (1<<0) | (1<<1);
		
		//��λ	
		nand_reset();
	
	
}


/* ҳ��nandflash ÿһ��64ҳ ÿһҳ�Ĵ�СΪ2K */
void NF_Page_Read(unsigned long addr, unsigned char* buff)
{
		int i;
		
		//ѡ��nandflashоƬ	���ܶ�����д������Ҫѡ��оƬ
		select_chip();
		
		//���RnB
		clear_RnB();
		
		//��������0x00  
		send_cmd(0x00);
		
		//5�����ڷ��͵�ַ  2�е�ַ  3�е�ַ
		//2�е�ַ		�е�ַ��ʾ��ҳ�ϵ�ƫ��  ��Ϊ�˲�����ҳ����  ����ƫ�ƾ�Ϊ0
		send_addr(0x00);
		send_addr(0x00);
		
		//3�е�ַ		�е�ַ��ʾ����ҳ�ı�� 
		send_addr(addr & 0xff);
		send_addr((addr>>8) & 0xff);
		send_addr((addr>>16) & 0xff);
		
		
		//��������0x30
		send_cmd(0x30);
		
		//�ȴ�RnB		�ȴ�RnB����ζ����Ҫ�����RnBS
		wait_RnB();  //�ȴ�nandflash��ǰ������ø��²���Ч
	
		//��ȡ����
		for(i=0; i<2048; i++) {
			
				buff[i] = rNFDATA;						
		}
		
		//ȡ��ѡ��nandflashоƬ	ѡ��оƬ����Ҫȡ��ѡ��
		deselect_chip();	
}

/***********************************************************************
*************   Nand������Ƶ�SRAM(�ڴ�)����    ************************
* start.S��ԭ����copy_to_ramʵ�ְ�SRAM��4K������Ƶ�SRAM(�ڴ�)��
* ����Ҫʵ�ְ�Nand�����еĴ�����Ƶ�SRAM(�ڴ�)��ע��Ҫ�ȳ�ʼ����ջ
* start_addr [11:0]�е�ַ [30:12]�е�ַ 
******************************************************** ***************/
void nand_to_ram(unsigned long start_addr, unsigned char* sdram_addr, int size)
{
		int i;		//ҳ��
		
		for(i=(start_addr>>11); size>0;) {
			
				NF_Page_Read(i, sdram_addr);
				size -= 2018;
				sdram_addr += 2048;
				i++;
		}
}


/* ҳ����nandflash ÿһ��64ҳ ÿһҳ�Ĵ�СΪ2K */
int NF_Erase(unsigned long addr)
{
		int ret;
		
		//ѡ��оƬ   ���ܶ�����д������Ҫѡ��оƬ
		select_chip();
		
		//���RnB
		clear_RnB();
		
		//��������0x60
		send_cmd(0x60);
		
		//�����е�ַ
		send_addr(addr & 0xff);				//addr ��ַ��Чλ[18:0]
		send_addr((addr>>8) & 0xff);
		send_addr((addr>>16) & 0xff);
		
		//��������0xd0
		send_cmd(0xd0);
		
		//�ȴ�RnB
		wait_RnB();		//�ȴ�nandflash��ǰ������ø��²���Ч
		
		//��������0x70
		send_cmd(0x70);
		
		//��ȡ�������
		ret = rNFDATA;
	
		//ȡ��ѡ��оƬ	ѡ��оƬ����Ҫȡ��ѡ��
		deselect_chip();
		
		return ret;
}


/* ҳдnandflash ÿһ��64ҳ ÿһҳ�Ĵ�СΪ2K */
int NF_Page_Write(unsigned long addr, unsigned char* buff)
{
		int i;
		int ret;
		
		//ѡ��оƬ   ���ܶ�����д������Ҫѡ��оƬ
		select_chip();
		
		//���RnB
		clear_RnB();
		
		//��������0x80
		send_cmd(0x80);
		
		
		//5�����ڽ��յ�ַ  2�е�ַ  3�е�ַ
		//2�е�ַ		�е�ַ��ʾ��ҳ�ϵ�ƫ��  ��Ϊ�˲�����ҳ����  ����ƫ�ƾ�Ϊ0
		send_addr(0x00);
		send_addr(0x00);
		
		//3�е�ַ		�е�ַ��ʾ����ҳ�ı�� 
		send_addr(addr & 0xff);				//addr ��ַ��Чλ[18:0]
		send_addr((addr>>8) & 0xff);
		send_addr((addr>>16) & 0xff);
		
		
		//д����
		for(i=0; i<2048; i++) {
			
				rNFDATA = buff[i];
		}	
		
		//��������0x10
		send_cmd(0x10);
		
		//�ȴ�RnB  �ȴ�RnB����ζ����Ҫ�����RnBS
		wait_RnB();		//�ȴ�nandflash��ǰ������ø��²���Ч
		
		//��������0x70
		send_cmd(0x70);
		
		//����д�����	
		ret = rNFDATA;
		
	
		//ȡ��ѡ��оƬ	ѡ��оƬ����Ҫȡ��ѡ��
		deselect_chip();
		
		return ret;
}