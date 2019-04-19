name = snif
obj-m += $(name).o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
upload:
	sudo insmod $(name).ko
end:
	sudo rmmod $(name)
	make clean
start:
	make all
	make upload
test:
	make upload
	make end