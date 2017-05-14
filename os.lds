OUTPUT_ARCH(arm)  /*指明输出程序的格式是arm */
ENTRY(_start)     /*指明程序入口为_start */
SECTIONS {
	. = 0x30008000;   /*定义链接地址 2440 0x30008000 */
					  /*6410 0x50008000、210 x20008000*/
	. = ALIGN(4);
	.text :
	{
	start.o (.text)
	*(.text)
	}

	. = ALIGN(4);
	.data : 
	{
	*(.data)
	}
	
	. = ALIGN(4);
	bss_start = .;
	.bss : 
	{
	*(.bss) 
	}
	bss_end = .;
}
