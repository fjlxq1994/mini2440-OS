objs := div64.o ctype.o muldi3.o printf.o string.o vsprintf.o 

all : $(objs)
	arm-linux-ld -r -o lib.o $^
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^		##CFLAGS通常来源于顶层文件
	
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o	
	
