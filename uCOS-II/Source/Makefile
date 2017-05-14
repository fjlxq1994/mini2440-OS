objs := os_core.o os_dbg_r.o os_flag.o os_mbox.o os_mem.o os_mutex.o os_q.o 
objs += os_sem.o os_task.o os_time.o os_tmr.o

##6410 add mem_6410.o  210 add mem_210.o

LDFLAG  := -L/opt/arm/4.3.2/lib/gcc/arm-none-linux-gnueabi/4.3.2
LDFLAG  += -L/opt/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/lib
LDFLAG  += --start-group -lgcc -lgcc_eh -lgcov -lc --end-group

all : $(objs)
	arm-linux-ld -r -o  ucos-II.o $^ $(LDFLAG) 
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -c $^		##CFLAGS通常来源于顶层文件
%.o : %.S
	arm-linux-gcc ${CFLAGS} -c $^

clean:
	rm -f *.o		
	
