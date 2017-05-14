#dev/dev.o 表示dev目录下的dev.o,lib也是同样的道理
OBJS := start.o dev/dev.o lib/lib.o apps/main.o uCOS-II/Source/ucos-II.o
OBJS += uCOS-II/Ports/ports.o int/int.o shell/command.o

CFLAGS := -fno-builtin -Wall 
CFLAGS += -I$(shell pwd)/apps
CFLAGS += -I$(shell pwd)/dev
CFLAGS += -I$(shell pwd)/include
CFLAGS += -I$(shell pwd)/lib
CFLAGS += -I$(shell pwd)/uCOS-II/Cfg
CFLAGS += -I$(shell pwd)/uCOS-II/Ports
CFLAGS += -I$(shell pwd)/uCOS-II/Source
CFLAGS += -I$(shell pwd)/int
CFLAGS += -I$(shell pwd)/shell

LDFLAG  := -Bstatic -Tos.lds
LDFLAG  += -L/opt/arm/4.3.2/lib/gcc/arm-none-linux-gnueabi/4.3.2
LDFLAG  += -L/opt/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/lib
LDFLAG  += --start-group -lgcc -lgcc_eh -lgcov -lc --end-group

export CFLAGS
	
os.bin : os.elf
	arm-linux-objcopy -O binary os.elf os.bin
	arm-linux-objdump -D -S os.elf >os.dis
	
os.elf : $(OBJS)	
	arm-linux-ld $(LDFLAG) -o os.elf $^


#指明所有.o文件均由对应的.s文件生成 	$^表示所有依赖
%.o : %.S    
	arm-linux-gcc -g -c $^
	
%.o : %.c
	arm-linux-gcc $(CFLAGS) -c $^

#make -C path 表示进入path，使用path下的makefile	
lib/lib.o : 
	make -C lib all
	
dev/dev.o :
	make -C dev all

apps/main.o :
	make -C apps all

uCOS-II/Source/ucos-II.o:
	make -C uCOS-II/Source all

uCOS-II/Ports/ports.o:
	make -C uCOS-II/Ports all

int/int.o:
	make -C int all
	
shell/command.o:
	make -C shell all

.PHONY: clean
clean:
	rm -f *.o *.elf *.bin *.dis
	make -C lib clean
	make -C dev clean
	make -C apps clean
	make -C uCOS-II/Source clean
	make -C uCOS-II/Ports clean
	make -C int clean
	make -C shell clean

