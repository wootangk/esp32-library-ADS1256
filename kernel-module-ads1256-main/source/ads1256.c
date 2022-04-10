#include <linux/module.h>
#include <linux/device.h>

static struct class *dev_class = NULL;

int init_module(void)
{
    printk("Hello World!\n");

    dev_class = class_create(THIS_MODULE, "hello");
  
    return 0;
}

void cleanup_module(void)
{
    class_destroy(dev_class);
    printk("Goodbye Cruel World!\n");
}

MODULE_LICENSE("GPL");