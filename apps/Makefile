objs := main.c
##6410 add mem_6410.o  210 add mem_210.o

all : $(objs)
	arm-linux-gcc $(CFLAGS)	-c $^ -o main.o
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^	
##CFLAGS通常来源于顶层文件
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o	
