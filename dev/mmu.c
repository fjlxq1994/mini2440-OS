



void create_page_table()
{
	unsigned long* ttb = (unsigned long*)0x30000000;	//基地址
	unsigned long vaddr, paddr;	
	
	vaddr = 0xa0000000;		//虚拟地址
	paddr = 0x56000000;		//物理地址
	
}