#define		rGPGCON		*((volatile unsigned long*)0x56000060)
#define		rGPGDAT		*((volatile unsigned long*)0x56000064)

#define		GPG0_EINT8		(0x2<<0)		  //key1
#define		GPG3_EINT11		(0x2<<6)		  //key2
#define		GPG5_EINT13		(0x2<<10)			//key3
#define		GPG6_EINT14		(0x2<<12)			//key4


void key_init()
{
		rGPGCON	&= ~((0x3<<0) | (0x3<<6) | (0x3<<10) | (0x3<<12));
		rGPGCON |= (GPG0_EINT8 | GPG3_EINT11 | GPG5_EINT13 | GPG6_EINT14);			
}	