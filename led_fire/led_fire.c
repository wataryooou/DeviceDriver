#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h> 
#include <asm/uaccess.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

MODULE_AUTHOR("Ryou Watanabe");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

double sec = 1.0;
int flag=0;
static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;

static struct task_struct *kthread_tsk;
static void my_kthread_main(void)
{
       set_current_state(TASK_INTERRUPTIBLE);
       //schedule_timeout(0.1*HZ);
	   if(flag == 0){
           schedule_timeout(0.1*HZ);
		   flag = 1;
           gpio_base[10] = 1 << 25;      
	   }else if(flag == 1){
       	   schedule_timeout(1.0*HZ);
		   flag = 0;
           gpio_base[7] = 1 << 25;      
	   }
	   printk("kthread:%ld\n", jiffies);
}
static int my_kthread(void *arg)
{
       printk("HZ:%d\n", HZ);
       while (!kthread_should_stop()) {
            my_kthread_main();
        }
       return 0;
}
/*
static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
	        printk(KERN_INFO "led_write is called\n");
			return 1;
}
*/


static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos)
{
	int i;
	char c;
    if(copy_from_user(&c,buf,sizeof(char)))
	    return -EFAULT;
	

	if(c == '0'){
		gpio_base[10] = 1 << 25;
		printk(KERN_INFO "\e[31mLED OFF\e[m\n");
//		sec = 2;
	}else if(c == '1'){
		gpio_base[7] = 1 << 25;
		printk(KERN_INFO "\e[31mLED ON\e[m\n");
//		sec = 1;
	}else if(c == 'w'){
		for(i=0;i<5;i++){
		    printk(KERN_INFO "%d\n",i);
            set_current_state(TASK_INTERRUPTIBLE);
       	    schedule_timeout(i*HZ);
		}
	}/*else if(c == 'l'){
		for(i=0;i<100;){
            set_current_state(TASK_INTERRUPTIBLE);
       	    schedule_timeout(i*HZ);
			i=i+0.1;
		}
	}*/
	printk(KERN_INFO "receive \e[31m%c\e[m\n",c);
	return 1;
}


static ssize_t sushi_read(struct file* filp, char* buf, size_t count, loff_t* pos)
{
	int size = 0;
	char sushi[] = {0xF0,0x9F,0x8D,0xA3,0x0A};
	if(copy_to_user(buf+size,(const char *)sushi, sizeof(sushi))){
		printk( KERN_INFO "sushi : copy_to_user failed\n" );
		return -EFAULT;
	}
	size += sizeof(sushi);
	return size;
}


static struct file_operations led_fops = {
	        .owner = THIS_MODULE,
	        .write = led_write,
			.read = sushi_read
};



static int __init init_mod(void)
{
	int retval;
	gpio_base = ioremap_nocache(0x3f200000, 0xA0);

	const u32 led = 25;
	const u32 index = led/10;
	const u32 shift = (led%10)*3;
	const u32 mask = ~(0x7 << shift);
	gpio_base[index] = (gpio_base[index] & mask) | (0x1 << shift);

	retval =  alloc_chrdev_region(&dev, 0, 1, "led_fire");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev_region failed.\n");
	return retval;
	}
	printk(KERN_INFO "%s is loaded. major:%d\n",__FILE__,MAJOR(dev));
	printk(KERN_INFO "%s is loaded.\n",__FILE__);
	cdev_init(&cdv, &led_fops);
    retval = cdev_add(&cdv, dev, 1);
    if(retval < 0){
		printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
        return retval;
	}
    kthread_tsk = kthread_run(my_kthread, NULL, "param %s&%d", "test", 123);
    if (IS_ERR(kthread_tsk))
        return -1;
    printk("pid->%d:prio->%d:comm->%s\n",
        kthread_tsk->pid,
        kthread_tsk->static_prio,
		kthread_tsk->comm);
	return 0;

	cls = class_create(THIS_MODULE,"led_fire");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create failed.");
		return PTR_ERR(cls);
	}
    device_create(cls, NULL, dev, NULL, "led_fire%d",MINOR(dev));
	return 0;
}

static void __exit cleanup_mod(void)
{
	cdev_del(&cdv);
	printk(KERN_INFO "%s is unloaded.\n",__FILE__);
	unregister_chrdev_region(dev, 1);
    kthread_stop(kthread_tsk);
	printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
    device_destroy(cls, dev);
    class_destroy(cls);
}

module_init(init_mod);
module_exit(cleanup_mod);
