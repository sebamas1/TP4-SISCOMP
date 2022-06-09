#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/slab.h>

static dev_t first;       // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl;  // Global variable for the device class

static char c;

static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "TP_FINAL: open()\n");
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "TP_FINAL: close()\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "TP_FINAL: read()\n");
    return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    loff_t pos;

    // Export the desired pin by writing to /sys/class/gpio/export
    struct file *fd = filp_open("/sys/class/gpio/export", O_WRONLY, 0644);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/export\n");
        return PTR_ERR(fd);
    }
    char *gpio_num = "24";
    kernel_write(fd, gpio_num, strlen(gpio_num), &pos);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error writing to /sys/class/gpio/export\n");
        return PTR_ERR(fd);
    }
    filp_close(fd, NULL);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction
    fd = filp_open("/sys/class/gpio/gpio24/direction", O_WRONLY, 0644);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/gpio24/direction\n");
        return PTR_ERR(fd);
    }
    char *gpio_dir = "out";
    kernel_write(fd, gpio_dir, strlen(gpio_dir), &pos);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error writing to /sys/class/gpio/gpio24/direction\n");
        return PTR_ERR(fd);
    }
    filp_close(fd, NULL);

    fd = filp_open("/sys/class/gpio/export", O_WRONLY, 0644);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/export\n");
        return PTR_ERR(fd);
    }
    gpio_num = "23";
    kernel_write(fd, gpio_num, strlen(gpio_num), &pos);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error writing to /sys/class/gpio/export\n");
        return PTR_ERR(fd);
    }
    filp_close(fd, NULL);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio24/direction
    fd = filp_open("/sys/class/gpio/gpio23/direction", O_WRONLY, 0644);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/gpio23/direction\n");
        return PTR_ERR(fd);
    }
    gpio_dir = "in";
    kernel_write(fd, gpio_dir, strlen(gpio_dir), &pos);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error writing to /sys/class/gpio/gpio23/direction\n");
        return PTR_ERR(fd);
    }
    filp_close(fd, NULL);

        char *gpio_val;
        // aloca memoria para leer el valor del pin
        gpio_val = kmalloc(sizeof(char), GFP_KERNEL);

    int i = 0;
    while (i < 10)
    {
        fd = filp_open("/sys/class/gpio/gpio23/value", O_RDONLY, 0644);
        if (IS_ERR(fd))
        {
            printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/gpio23/value\n");
            return PTR_ERR(fd);
        }

        loff_t hola = 0;
        kernel_read(fd, gpio_val, 1, &hola);
        if (IS_ERR(fd))
        {
            printk(KERN_INFO "TP_FINAL: Error reading from /sys/class/gpio/gpio23/value\n");
            return PTR_ERR(fd);
        }
        printk(KERN_INFO "TP_FINAL: gpio_val = %d\n", *gpio_val);

        filp_close(fd, NULL);
        if (gpio_val[0] == '0')
        {
            // Set the pin to be high by writing "1" to /sys/class/gpio/gpio24/value
            fd = filp_open("/sys/class/gpio/gpio24/value", O_WRONLY, 0644);

            if (IS_ERR(fd))
            {
                printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/gpio24/value\n");
                return PTR_ERR(fd);
            }
            char *gpio_val = "1";

            kernel_write(fd, gpio_val, strlen(gpio_val), &pos);
            if (IS_ERR(fd))
            {
                printk(KERN_INFO "TP_FINAL: Error writing to /sys/class/gpio/gpio24/value\n");
                return PTR_ERR(fd);
            }
            filp_close(fd, NULL);
	    printk(KERN_INFO "DEBERIA PRENDER\n");
        }
        else
        {
            // Set the pin to be high by writing "1" to /sys/class/gpio/gpio24/value
            fd = filp_open("/sys/class/gpio/gpio24/value", O_WRONLY, 0644);

            if (IS_ERR(fd))
            {
                printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/gpio24/value\n");
                return PTR_ERR(fd);
            }
            char *gpio_val = "0";

            kernel_write(fd, gpio_val, strlen(gpio_val), &pos);
            if (IS_ERR(fd))
            {
                printk(KERN_INFO "TP_FINAL: Error writing to /sys/class/gpio/gpio24/value\n");
                return PTR_ERR(fd);
            }
            filp_close(fd, NULL);
	    printk(KERN_INFO "DEBERIA APAGAR\n");
        }
        msleep(1500);
        i++;
	memset(gpio_val, 0, sizeof(gpio_val));
    }

    // Unexport the pin by writing to /sys/class/gpio/unexport
    fd = filp_open("/sys/class/gpio/unexport", O_WRONLY, 0644);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error opening /sys/class/gpio/unexport\n");
        return PTR_ERR(fd);
    }
    char *gpio_unexport = "24";
    kernel_write(fd, gpio_unexport, strlen(gpio_unexport), &pos);
    if (IS_ERR(fd))
    {
        printk(KERN_INFO "TP_FINAL: Error writing to /sys/class/gpio/unexport\n");
        return PTR_ERR(fd);
    }
    filp_close(fd, NULL);

    printk(KERN_INFO "finalizando write\n");
    // And exit
    return 1;
}

static struct file_operations pugs_fops =
    {
        .owner = THIS_MODULE,
        .open = my_open,
        .release = my_close,
        .read = my_read,
        .write = my_write};

static int __init drv2_init(void) /* Constructor */
{
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "TP_FINAL: Registrado exitosamente..!!\n");

    if ((ret = alloc_chrdev_region(&first, 0, 1, "TP_FINAL")) < 0)
    {
        return ret;
    }

    if (IS_ERR(cl = class_create(THIS_MODULE, "TP_FINAL_drive")))
    {
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }

    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "TP_FINAL")))
    {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&c_dev, &pugs_fops);
    if ((ret = cdev_add(&c_dev, first, 1)) < 0)
    {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }
    return 0;
}

static void __exit drv2_exit(void) /* Destructor */
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "TP_FINAL: dice Adios mundo cruel..!!\n");
}

module_init(drv2_init);
module_exit(drv2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Amantes del assembler");
MODULE_DESCRIPTION("TP FinaL de SdeC");
