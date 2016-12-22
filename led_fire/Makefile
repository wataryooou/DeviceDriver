obj-m:=led_fire.o

led_fire.ko: led_fire.c
	make -C /usr/src/linux M=`pwd` V=1 modules

clean:
	make -C /usr/src/linux M=`pwd` V=1 clean 
