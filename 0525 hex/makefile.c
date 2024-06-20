obj-m += hex.o
all:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
        mknod /dev/hex c 240 0
        chmod a+w /dev/hex
        insmod hex.ko
