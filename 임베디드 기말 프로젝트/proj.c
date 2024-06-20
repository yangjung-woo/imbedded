#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SangKyun Yun");
MODULE_DESCRIPTION("Seven Segment dispaly and LED flashing");

void init_add_timer(void);
void remove_timer(void);
void hex_timer_function(unsigned long ptr);

#define base_lwFPGA 0xFF200000
#define len_lwFPGA  0x200000

#define addr_LED  0
#define addr_HEX0 0x20
#define addr_HEX1 0x30
#define addr_SW   0x40

#define addr_KEY  0x50
#define	INTMASK   0x8
#define	EDGE      0xC

#define	IRQ_KEYS  73

static void *mem_base;
static void *hex0_addr;
static void *hex1_addr;

void *lwbridgebase;
void *ledr_addr, *key_addr;
void *sw_addr;
int press;
int i, j, k, l;

static unsigned int data = -1;
static unsigned int mode = 0;

#define NOFILL 4
#define BLINK  8

unsigned int hex0, hex1;

int hex_conversion[16] = {
        0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
        0x7F,0x67,0x77,0x7C,0x39,0x5E,0x79,0x71,
};

static ssize_t hex_write (struct file *file, const char __user *buf, size_t count, loff_t *f_pos) {
        unsigned int hex_data = 0;
        unsigned int nofill = 0;

        get_user(hex_data, (unsigned int *)buf);
        hex_data = hex_data & 0xFFFFFF;
        data = hex_data;

        if (mode & NOFILL) nofill = 1;

        hex1 = 0;
        hex0 = hex_conversion[hex_data & 0xf];

        do{
                hex_data >>= 4;
                if (nofill && hex_data == 0) break;
                hex0 |= hex_conversion[hex_data &0xf]<<8;

                hex_data >>= 4;
                if (nofill && hex_data == 0) break;
                hex0 |= hex_conversion[hex_data & 0xf]<<16;

                hex_data >>= 4;
                if (nofill && hex_data == 0) break;
                hex0 |= hex_conversion[hex_data & 0xf]<<24;

                hex_data >>= 4;
                if(nofill && hex_data == 0) break;
                hex1 = hex_conversion[hex_data & 0xf];

                hex_data >>= 4;
                if (nofill && hex_data == 0) break;
                hex1 |= hex_conversion[hex_data &0xf]<<8;
        } while(0);

        iowrite32(hex0, hex0_addr);
        iowrite32(hex1, hex1_addr);

        return 4;
}

static ssize_t hex_read (struct file *file, char __user *buf ,size_t count, loff_t *f_pos) {
        put_user(data, (unsigned int *) buf);
        return 4;
}

static int hex_open(struct inode *minode, struct file *mfile) {
        return 0;
}

static int hex_release(struct inode *minode, struct file *mfile) {
        return 0;
}

static long hex_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
        unsigned int newcmd;

        newcmd = cmd;
        if ( (mode & BLINK) && !(newcmd & BLINK))
                remove_timer();
        else if ( !(mode&BLINK) && (newcmd& BLINK))
                init_add_timer();
        mode= newcmd;

        return 0;
}

static struct file_operations hex_fops = {
        .read = hex_read,
        .write = hex_write,
        .open = hex_open,
        .release = hex_release,
        .unlocked_ioctl  = hex_ioctl,
};



static struct cdev hex_cdev;
static struct class *cl;
static dev_t dev_no;

#define DEVICE_NAME "hex"


irq_handler_t irq_handler(int irq, void	*dev_id, struct	pt_regs	*regs)
{  unsigned int sw =0;
   unsigned int hex_raw =0;
   unsigned int hex_new_0 =0;
   unsigned int hex_new_1 =0;
   unsigned int hex_new_2=0;
   unsigned int hex_new_3 =0;
   
   sw= ioread32(sw_addr);
  iowrite32(sw, ledr_addr);


    unsigned int value;
    press = ioread32(key_addr + EDGE);
    iowrite32(0xf, key_addr + EDGE);

    int hex_data=0x0000;
    hex0 =0;
    hex_raw= ioread32(hex0_addr); 
 
 // hex_raw -> 0~F 의 값으로 변환 
   hex_new_0 = hex_raw & 0xFF;
   switch(hex_new_0){
   case 0x3F:  hex_new_0 = 0x0;
   break;
 case 0x06:  hex_new_0 = 0x1;
   break;
 case 0x5B:  hex_new_0 = 0x2;
   break;
 case 0x4F:  hex_new_0 = 0x3;
   break;
 case 0x66:  hex_new_0 = 0x4;
   break;
 case 0x6D:  hex_new_0 = 0x5;
   break;
 case 0x7D:  hex_new_0 = 0x6;
   break;
 case 0x07:  hex_new_0 = 0x7;
   break;
 case 0x7F:  hex_new_0 = 0x8;
   break;
 case 0x67:  hex_new_0 = 0x9;
   break;
 case 0x77:  hex_new_0 = 0xA;
   break;
 case 0x7C:  hex_new_0 = 0xB;
   break;
 case 0x39:  hex_new_0 = 0xC;
   break;
 case 0x5E:  hex_new_0 = 0xD;
   break;
 case 0x79:  hex_new_0 = 0xE;
   break;
 case 0x71:  hex_new_0 = 0xF;
   break;
   }

   hex_raw >>=8;
   hex_new_1 = hex_raw & 0xFF;
 switch(hex_new_1){
   case 0x3F:  hex_new_1 = 0x0;
   break;
 case 0x06:  hex_new_1 = 0x1;
   break;
 case 0x5B:  hex_new_1 = 0x2;
   break;
 case 0x4F:  hex_new_1 = 0x3;
   break;
 case 0x66:  hex_new_1 = 0x4;
   break;
 case 0x6D:  hex_new_1 = 0x5;
   break;
 case 0x7D:  hex_new_1= 0x6;
   break;
 case 0x07:  hex_new_1 = 0x7;
   break;
 case 0x7F:  hex_new_1 = 0x8;
   break;
 case 0x67:  hex_new_1 = 0x9;
   break;
 case 0x77:  hex_new_1 = 0xA;
   break;
 case 0x7C:  hex_new_1 = 0xB;
   break;
 case 0x39:  hex_new_1 = 0xC;
   break;
 case 0x5E:  hex_new_1 = 0xD;
   break;
 case 0x79:  hex_new_1 = 0xE;
   break;
 case 0x71:  hex_new_1 = 0xF;
   break;
   }
   hex_raw >>=8;
   hex_new_2 = hex_raw & 0xFF;
 switch(hex_new_2){
   case 0x3F:  hex_new_2 = 0x0;
   break;
 case 0x06:  hex_new_2 = 0x1;
   break;
 case 0x5B:  hex_new_2 = 0x2;
   break;
 case 0x4F:  hex_new_2 = 0x3;
   break;
 case 0x66:  hex_new_2 = 0x4;
   break;
 case 0x6D:  hex_new_2 = 0x5;
   break;
 case 0x7D:  hex_new_2 = 0x6;
   break;
 case 0x07:  hex_new_2 = 0x7;
   break;
 case 0x7F:  hex_new_2 = 0x8;
   break;
 case 0x67:  hex_new_2 = 0x9;
   break;
 case 0x77:  hex_new_2 = 0xA;
   break;
 case 0x7C:  hex_new_2 = 0xB;
   break;
 case 0x39:  hex_new_2 = 0xC;
   break;
 case 0x5E:  hex_new_2 = 0xD;
   break;
 case 0x79:  hex_new_2 = 0xE;
   break;
 case 0x71:  hex_new_2 = 0xF;
   break;
   }
   hex_raw >>=8;
   hex_new_3 = hex_raw & 0xFF;
 switch(hex_new_3){
   case 0x3F:  hex_new_3 = 0x0;
   break;
 case 0x06:  hex_new_3 = 0x1;
   break;
 case 0x5B:  hex_new_3 = 0x2;
   break;
 case 0x4F:  hex_new_3 = 0x3;
   break;
 case 0x66:  hex_new_3 = 0x4;
   break;
 case 0x6D:  hex_new_3 = 0x5;
   break;
 case 0x7D:  hex_new_3 = 0x6;
   break;
 case 0x07:  hex_new_3 = 0x7;
   break;
 case 0x7F:  hex_new_3 = 0x8;
   break;
 case 0x67:  hex_new_3 = 0x9;
   break;
 case 0x77:  hex_new_3 = 0xA;
   break;
 case 0x7C:  hex_new_3 = 0xB;
   break;
 case 0x39:  hex_new_3 = 0xC;
   break;
 case 0x5E:  hex_new_3 = 0xD;
   break;
 case 0x79:  hex_new_3 = 0xE;
   break;
 case 0x71:  hex_new_3 = 0xF;
   break;
   }
   // hex_new 1 2 3 0~F 저장
   hex_data = hex_new_3;
   hex_data <<= 4;
   hex_data |= hex_new_2;
   hex_data <<= 4;
   hex_data |= hex_new_1;
   hex_data <<= 4;
   hex_data |= hex_new_0;     // hex data= 0x 1234 .. 0x 5263... 저장 

    value= ioread32(sw_addr);
  
     hex1=0;
    //hex0 = hex_conversion[hex_data & 0xf];
      
        if(press==0x8) {//+
          hex_data= hex_data+ value;
            if( hex_data > 0xFFFF) hex1 = 0x7900;
            
	hex0=hex_conversion[hex_data & 0xf];
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data &0xf]<<8;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<16;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<24;
             iowrite32(hex0, hex0_addr);
	iowrite32(hex1, hex1_addr);
        }

  
        if (press==0x4) { //-
            hex_data= hex_data- value;
              if (hex_data < 0x0) hex1= 0x0079;
	hex0=hex_conversion[hex_data & 0xf];
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data &0xf]<<8;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<16;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<24;
         iowrite32(hex0, hex0_addr);
       iowrite32(hex1, hex1_addr);
        } 

  
        if (press==0x2) { //   나누기
             hex_data= hex_data / value;
	hex0=hex_conversion[hex_data & 0xf];
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data &0xf]<<8;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<16;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<24;
         iowrite32(hex0, hex0_addr);
        iowrite32(hex1, hex1_addr);
        }

    
        if (press==0x1) { // x     
              hex_data= hex_data* value;
             if( hex_data > 0xFFFF) hex1 = 0x7900;
	hex0=hex_conversion[hex_data & 0xf];
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data &0xf]<<8;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<16;
              hex_data >>= 4;
              hex0 |= hex_conversion[hex_data & 0xf]<<24;
         iowrite32(hex0, hex0_addr);
      iowrite32(hex1, hex1_addr);
        }
    
    return (irq_handler_t) IRQ_HANDLED;
}
static int __init init(void) {
        if (alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME) < 0) {
            printk(KERN_ERR "alloc_chrdev_region error\n");
        return -1;
        }
        cdev_init(&hex_cdev,&hex_fops);

        if(cdev_add(&hex_cdev, dev_no, 1) < 0) {
        printk(KERN_ERR "cdev_add() error \n");
        goto unreg_chrdev;
        }

        cl =class_create (THIS_MODULE, DEVICE_NAME);
        if(cl==NULL){
        printk(KERN_ALERT "class_create() error\n");
        goto unreg_chrdev;
        }

        if(device_create(cl, NULL, dev_no, NULL, DEVICE_NAME) == NULL) {
        printk(KERN_ALERT "device_create error\n");
        goto unreg_class;
        }

        mem_base = ioremap_nocache(base_lwFPGA, len_lwFPGA);
        if(mem_base == NULL) {
        printk(KERN_ERR "ioremap_nocache() error \n");
        goto un_device;
        }

        printk("Device: %s Major %d %x\n", DEVICE_NAME, MAJOR(dev_no), dev_no);
        hex0_addr = mem_base + addr_HEX0;
        hex1_addr = mem_base + addr_HEX1;

        lwbridgebase = ioremap_nocache(base_lwFPGA, len_lwFPGA);
	    ledr_addr = lwbridgebase + addr_LED;
	    key_addr = lwbridgebase + addr_KEY;
	    sw_addr = lwbridgebase + addr_SW;
        mdelay(400);
        iowrite32(0x3ff, ledr_addr);
        mdelay(400);
        iowrite32(0x0, ledr_addr);
        mdelay(400);
        iowrite32(0x3ff, ledr_addr);
        mdelay(400);
        iowrite32(0x0, ledr_addr);

        iowrite32(0xf, key_addr	+ INTMASK);
        iowrite32(0xf, key_addr + EDGE);

        return request_irq(IRQ_KEYS, (irq_handler_t)irq_handler,
				IRQF_SHARED, "pushbutton_irq_handler",
                (void *)(irq_handler));

// error
un_device:
        device_destroy(cl,dev_no);
unreg_class:
        class_destroy(cl);
unreg_chrdev:
        unregister_chrdev_region(dev_no,1);
        return -1;
}
static void __exit exit (void) {
        iowrite32(0, hex0_addr);
        iowrite32(0, hex1_addr);

        iowrite32(0x0, ledr_addr);
        free_irq(IRQ_KEYS, (void*)irq_handler);
        
        if( mode & BLINK)
                remove_timer();

        iounmap(mem_base);
        device_destroy(cl,dev_no);
        class_destroy(cl);
        unregister_chrdev_region(dev_no, 1);
        printk(" %s unregistered.\n", DEVICE_NAME);
}

module_init(init);
module_exit(exit);

static int turnoff = 0;
static struct timer_list hex_timer;

void init_add_timer (void) {
        init_timer(&hex_timer);

        hex_timer.function = hex_timer_function;
        hex_timer.expires = jiffies + HZ; //after 1 sec
        hex_timer.data = 0;

        add_timer(&hex_timer);
}

void remove_timer (void) {
        del_timer(&hex_timer);
}

void hex_timer_function(unsigned long ptr) {
        if(!(mode & BLINK)) return;
        turnoff = !turnoff;
        if(turnoff) {
                iowrite32(0,hex0_addr);
                iowrite32(0,hex1_addr);
        } else {
                iowrite32(hex0, hex0_addr);
                iowrite32(hex1, hex1_addr);
        }

        init_add_timer();
}
