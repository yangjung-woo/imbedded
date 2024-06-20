#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
int main(void)
{
    int dev, data ,rdata;
    dev= open("/dev/led",O_RDWR);
    if(dev<0)
    {
        fprintf(stderr,"Cannot open LED device\n");
        return 1;
    }
    printf("Input LED data (hex):");
    scanf("%x",&data);
    write(dev,&data,4);
    read(dev,&rdata,4);
    printf("led data = %x\n",rdata);
    return 0;
}
