//DMA initial source Registers
#define		rDISRC0			(*(volatile unsigned long*)0x4b000000) 
#define		rDISRC1			(*(volatile unsigned long*)0x4b000040)
#define		rDISRC2			(*(volatile unsigned long*)0x4b000080)
#define		rDISRC3			(*(volatile unsigned long*)0x4b0000c0)
//DMA initial source control Registers
#define		rDISRCC0		(*(volatile unsigned long*)0x4b000004)
#define		rDISRCC1		(*(volatile unsigned long*)0x4b000044)
#define		rDISRCC2		(*(volatile unsigned long*)0x4b000084)
#define		rDISRCC3		(*(volatile unsigned long*)0x4b0000c4)
//DMA initial destination Registers
#define		rDIDST0			(*(volatile unsigned long*)0x4b000008)
#define		rDIDST1			(*(volatile unsigned long*)0x4b000048)
#define		rDIDST2			(*(volatile unsigned long*)0x4b000088)
#define		rDIDST3			(*(volatile unsigned long*)0x4b0000b8)
#define		rDIDSTC0		(*(volatile unsigned long*)0x4b00000c)
#define		rDIDSTC1		(*(volatile unsigned long*)0x4b00004c)
#define		rDIDSTC2		(*(volatile unsigned long*)0x4b00008c)
#define		rDIDSTC3		(*(volatile unsigned long*)0x4b0000cc)

#define		rDCON0			(*(volatile unsigned long*)0x4b000010)
#define		rDCON1			(*(volatile unsigned long*)0x4b000050)
#define		rDCON2			(*(volatile unsigned long*)0x4b000090)
#define		rDCON3			(*(volatile unsigned long*)0x4b0000d0)

#define		rDMASKTRIG0 (*(volatile unsigned long*)0x4B000020)
#define		rDMASKTRIG1 (*(volatile unsigned long*)0x4B000060)
#define		rDMASKTRIG2 (*(volatile unsigned long*)0x4B0000a0)
#define		rDMASKTRIG3 (*(volatile unsigned long*)0x4B0000e0)

#define		rUTXH0			(*(volatile unsigned long*)0x50000020)
#define		 UTXH0			(volatile unsigned long)0x50000020


char* buf = "hello world!";

//����DMAΪUart0�Զ��������ݣ�Uart������UCON0��ӦģʽҲ�����ó�DMAģʽ
void dma_init()
{
	
		//��ʼ��Դ��ַ
		rDISRC0 = (unsigned int)buf;		//װ��Դ��ַ
		rDISRCC0 = (0<<0) | (0<<1);     //Դ��ַ��������������)
																		//DMA����APB��Դ�豸ѡ��0
		
		//��ʼ��Ŀ�ĵ�ַ
		rDIDST0 = UTXH0;							//װ��Ŀ���豸��ַ
		rDIDSTC0 = (1<<1) | (1<<0);		//Ŀ�ĵ�ַ�Ǵ��ڣ��̶���
		
		 //Uart0 ����Ӳ��   �������ݴ�СĬ��8bit���øĶ�
		rDCON0 = (1<<24) | (1<<23) | (1<<22) | (12<<0);  //[19:0]�����ֽ���
																										
}

void dma_start()
{
		rDMASKTRIG0 = (1<<1);		 //��DMAͨ��
}