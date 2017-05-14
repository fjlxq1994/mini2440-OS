objs := mmu.o led.o key.o nandflash.o uart.o dma.o lcd.o bmp.o timer.o beep.o uart_lib.o

all : $(objs)
	arm-linux-ld -r -o dev.o $^
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^		##CFLAGS通常来源于顶层文件
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o		
	
