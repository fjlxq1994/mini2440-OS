objs := interrupt.o int_hook.o

all : $(objs)
	arm-linux-ld -r -o int.o $^
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^		##CFLAGS通常来源于顶层文件
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o	
