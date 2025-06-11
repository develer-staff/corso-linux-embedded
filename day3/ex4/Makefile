obj-m := hello_kgdb.o

all:
	make -C  $(KDIR) M=$(PWD) modules

clean:
	make -C  $(KDIR) M=$(PWD) clean
