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
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/string.h>

static dev_t first;       // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl;  // Global variable for the device class
int numbers[10];
int tipo = 0;

/*
Lee el pin pasado como argumento(string). Toma como argumento el pin.
*/
static char leer_pin(char *pin)
{
    char res;
    printk(KERN_INFO "TP_FINAL: Leyendo pin %s\n", pin);
    res = gpio_get_value(simple_strtoul(pin, NULL, 10));
    return res;
}

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
/*
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    int i;
    int res;
    char *valores = "0123456789";
    char *valor;
    printk(KERN_INFO "TP_FINAL: read()\n");
    if (*off >= 10)
        return 0;
    valor = kmalloc(sizeof(char), GFP_KERNEL);
    for (i = 0; i < 10; i++)
    {
        valor[i] = valores[i];
    }
    res = copy_to_user(buf, valor, 10);
    *off += 1;
    kfree(valor);
    return 105;
}*/

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{    
    printk(KERN_INFO "TP_FINAL: \n");
    printk(KERN_INFO "TP_FINAL: ---------------imprimir array de numeros---------------\n");
    char *valorNumero;
    char str[100];
    char *t;
    if(tipo == 1){
        t = "C";
       
    } else {
        t = "T";
       
    }
    
    sprintf(str, "%s%d,", t, numbers[0]);
    int i;
    int res;
    for(i=1; i<10; i++){
        char buffer_Numero[10];
        if(i==9){
             sprintf(buffer_Numero, "%d", numbers[9]);
        } else {
            sprintf(buffer_Numero, "%d,", numbers[i]);
        }
        strcat(str, buffer_Numero);
    }
    printk(KERN_INFO "TP_FINAL: %s\n", str);
    

    if (*off >= 10)
        return 0;


    int nr_bytes;
    nr_bytes = strlen(str);
    res = copy_to_user(buf, str, nr_bytes);
    *off += len;
    
    return nr_bytes;

}


static void toggle_led(char *pin)
{
    char value = leer_pin(pin);
    gpio_set_value(simple_strtoul(pin, NULL, 10), !value);
    if(tipo == 1){
        tipo = 0;
    } else {
        tipo = 1;
    }
}

static void generate_numbers(int min, int max){
    //array 10 posiciones para guardar los numeros aleatorios del 15 al 30
    
    int i;
    for(i=0; i<10; i++){        
        unsigned int rand;
        get_random_bytes(&rand, sizeof(rand));
        rand%=max;
        if(rand<min){
            rand+=min;
        }
        numbers[i] = (int)rand;
    }
        
}


/*
Esta funcion es un IRQ Handler, que se ejecuta cuando se recibe una interrupcion
por el GPIO. Toggles el LED.
*/
static irq_handler_t my_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
    printk(KERN_INFO "TP_FINAL: IRQ Handler\n");
    toggle_led("24");
    if(tipo == 0){ //Temperaturas
        generate_numbers(15, 30);
    } else { //Caudal
        generate_numbers(10000, 18000);
 
    }
    
  //  generate_numbers();
    return (irq_handler_t)IRQ_HANDLED;
}
/*
Configura e inicializa el pin pasado como argumento(string). Toma como argumento in o out
que es la direccion del pin. Si es in, lo configura como entrada, si es out, como salida.
*/
static void inicializar_pin(char *pin, char *in_out)
{
    if (strcmp(in_out, "in") == 0)
    {
        printk(KERN_INFO "TP_FINAL: Configurando pin %s como entrada\n", pin);
        gpio_direction_input(simple_strtoul(pin, NULL, 10));
    }
    else if (strcmp(in_out, "out") == 0)
    {
        printk(KERN_INFO "TP_FINAL: Configurando pin %s como salida\n", pin);
        gpio_direction_output(simple_strtoul(pin, NULL, 10), 0);
    }
    else
    {
        printk(KERN_INFO "TP_FINAL: Error en la configuracion del pin %s\n", pin);
    }
}
/*
Setea el pin al valor pasado como argumento(string). Toma como argumento el pin y el valor.
*/
static void setear_pin(char *pin, char *valor)
{
    if (strcmp(valor, "1") == 0)
    {
        printk(KERN_INFO "TP_FINAL: Seteando pin %s a 1\n", pin);
        gpio_set_value(simple_strtoul(pin, NULL, 10), 1);
    }
    else if (strcmp(valor, "0") == 0)
    {
        printk(KERN_INFO "TP_FINAL: Seteando pin %s a 0\n", pin);
        gpio_set_value(simple_strtoul(pin, NULL, 10), 0);
    }
    else
    {
        printk(KERN_INFO "TP_FINAL: Error en el seteo del pin %s\n", pin);
    }
}
/*
Activa interrupciones por el GPIO pasado como argumento(string). Toma como argumento el pin.
*/
static void activar_interrupcion(char *pin)
{
    int irq;
    printk(KERN_INFO "TP_FINAL: Activando interrupciones por el pin %s\n", pin);
    irq = gpio_to_irq(simple_strtoul(pin, NULL, 10));
    if (request_irq(irq, (irq_handler_t)my_irq_handler, IRQF_TRIGGER_RISING, "my_gpio_handler", NULL))
    {
        printk(KERN_INFO "TP_FINAL: Error al solicitar la interrupcion\n");
    }
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
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

    inicializar_pin("24", "out");
    setear_pin("24", "1");
    activar_interrupcion("23");
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
