obj-m += hellodev.o

all:
	make -C  $(KDIR) M=$(PWD) modules

clean:
	make -C  $(KDIR) M=$(PWD) clean