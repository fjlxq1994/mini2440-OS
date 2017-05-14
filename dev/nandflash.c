#define rNFCONF (*(volatile unsigned long*)0x4e000000)	//NAND Flash 配置寄存器 
#define rNFCONT (*(volatile unsigned long*)0x4e000004)	//NAND Flash 控制寄存器 
#define rNFCMD  (*(volatile unsigned char*)0x4e000008)	//NAND Flash 命令集寄存器
#define rNFDATA (*(volatile unsigned char*)0x4e000010)	//NAND Flash 数据寄存器 
#define rNFADDR (*(volatile unsigned char*)0x4e00000C)	//NAND Flash 地址集寄存器
#define rNFSTAT (*(volatile unsigned char*)0x4e000020)	//NAND Flash 运行状态寄存器

//HCLK 100M  10ns
#define		TACLS			1		//CLE 和ALE 持续值设置  >0ns  Duration = HCLK × TACLS  
#define		TWRPH0		2		//TWRPH0持续值   Duration(>12ns) = HCLK × ( TWRPH0 + 1 ) 
#define		TWRPH1		1		//TWRPH1持续值   Duration(>5ns ) = HCLK × ( TWRPH1 + 1 ) 


void select_chip()
{
		rNFCONT &= ~(1<<1);	  //使能片选
}

void deselect_chip()
{
		rNFCONT |= (1<<1);		//禁止片选
}

void clear_RnB()
{
		rNFSTAT |= (1<<2); 		//写1清除RnB位
}

void send_cmd(unsigned cmd)  //cmd 8bit
{	
		rNFCMD = cmd;					//接收命令
}

void send_addr(unsigned addr)	 //addr 8bit
{
		rNFADDR = addr;				//接收地址
}

void wait_RnB()  //给nandflash提供时间把前面的设置更新并生效
{
			while( !(rNFSTAT & (1<<2)) );	 
}


void nand_reset()
{
		//选中flash
		select_chip();
		
		//清除RnB
		clear_RnB();
		
		//发送0xff命令
		send_cmd(0xff);
		
		//等待RnB		
		wait_RnB();
		
		//取消选中flash	
		deselect_chip();
}


void nand_init()
{
		//初始化NFCONF
		rNFCONF = (TWRPH1<<4) | (TWRPH0<<8) | (TACLS<<12);
		
		//初始化NFCONT
		rNFCONT = (1<<0) | (1<<1);
		
		//复位	
		nand_reset();
	
	
}


/* 页读nandflash 每一块64页 每一页的大小为2K */
void NF_Page_Read(unsigned long addr, unsigned char* buff)
{
		int i;
		
		//选中nandflash芯片	不管读还是写，都需要选择芯片
		select_chip();
		
		//清除RnB
		clear_RnB();
		
		//发送命令0x00  
		send_cmd(0x00);
		
		//5个周期发送地址  2列地址  3行地址
		//2列地址		列地址表示在页上的偏移  因为此操作是页操作  所以偏移均为0
		send_addr(0x00);
		send_addr(0x00);
		
		//3行地址		行地址表示块中页的编号 
		send_addr(addr & 0xff);
		send_addr((addr>>8) & 0xff);
		send_addr((addr>>16) & 0xff);
		
		
		//发送命令0x30
		send_cmd(0x30);
		
		//等待RnB		等待RnB就意味着需要先清除RnBS
		wait_RnB();  //等待nandflash把前面的设置更新并生效
	
		//读取数据
		for(i=0; i<2048; i++) {
			
				buff[i] = rNFDATA;						
		}
		
		//取消选中nandflash芯片	选中芯片就需要取消选中
		deselect_chip();	
}

/***********************************************************************
*************   Nand代码搬移到SRAM(内存)函数    ************************
* start.S中原来的copy_to_ram实现把SRAM中4K程序搬移到SRAM(内存)中
* 现在要实现把Nand中所有的代码搬移到SRAM(内存)，注意要先初始化堆栈
* start_addr [11:0]列地址 [30:12]行地址 
******************************************************** ***************/
void nand_to_ram(unsigned long start_addr, unsigned char* sdram_addr, int size)
{
		int i;		//页号
		
		for(i=(start_addr>>11); size>0;) {
			
				NF_Page_Read(i, sdram_addr);
				size -= 2018;
				sdram_addr += 2048;
				i++;
		}
}


/* 页擦除nandflash 每一块64页 每一页的大小为2K */
int NF_Erase(unsigned long addr)
{
		int ret;
		
		//选中芯片   不管读还是写，都需要选择芯片
		select_chip();
		
		//清除RnB
		clear_RnB();
		
		//发送命令0x60
		send_cmd(0x60);
		
		//发送行地址
		send_addr(addr & 0xff);				//addr 地址有效位[18:0]
		send_addr((addr>>8) & 0xff);
		send_addr((addr>>16) & 0xff);
		
		//发送命令0xd0
		send_cmd(0xd0);
		
		//等待RnB
		wait_RnB();		//等待nandflash把前面的设置更新并生效
		
		//发送命令0x70
		send_cmd(0x70);
		
		//读取擦除结果
		ret = rNFDATA;
	
		//取消选中芯片	选中芯片就需要取消选中
		deselect_chip();
		
		return ret;
}


/* 页写nandflash 每一块64页 每一页的大小为2K */
int NF_Page_Write(unsigned long addr, unsigned char* buff)
{
		int i;
		int ret;
		
		//选中芯片   不管读还是写，都需要选择芯片
		select_chip();
		
		//清除RnB
		clear_RnB();
		
		//发送命令0x80
		send_cmd(0x80);
		
		
		//5个周期接收地址  2列地址  3行地址
		//2列地址		列地址表示在页上的偏移  因为此操作是页操作  所以偏移均为0
		send_addr(0x00);
		send_addr(0x00);
		
		//3行地址		行地址表示块中页的编号 
		send_addr(addr & 0xff);				//addr 地址有效位[18:0]
		send_addr((addr>>8) & 0xff);
		send_addr((addr>>16) & 0xff);
		
		
		//写数据
		for(i=0; i<2048; i++) {
			
				rNFDATA = buff[i];
		}	
		
		//发送命令0x10
		send_cmd(0x10);
		
		//等待RnB  等待RnB就意味着需要先清除RnBS
		wait_RnB();		//等待nandflash把前面的设置更新并生效
		
		//发送命令0x70
		send_cmd(0x70);
		
		//接收写入结束	
		ret = rNFDATA;
		
	
		//取消选中芯片	选中芯片就需要取消选中
		deselect_chip();
		
		return ret;
}