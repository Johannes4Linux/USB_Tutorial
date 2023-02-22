#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/gpio/driver.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A driver for my Atmega32U4 USB device");

#define VENDOR_ID 0x03eb
#define PRODUCT_ID 0x0001

struct m32u4_exp {
	struct gpio_chip chip;
	struct usb_device *usb_dev;
	uint8_t gpio_directions;
};

static void m32u4_exp_set(struct gpio_chip *chip, unsigned offset, int value);

static int m32u4_exp_get_direction(struct gpio_chip *chip, unsigned offset) {
	struct m32u4_exp *gpio = gpiochip_get_data(chip);
	return (gpio->gpio_directions & (1<<offset)) ? GPIO_LINE_DIRECTION_OUT : GPIO_LINE_DIRECTION_IN;
}


/**
 * @brief Sets the direction of the gpio pin nr. offset of gpiochip
 *
 * @param chip      gpiochip to set direction
 * @param offset    pin number
 * @param direction direction of gpio 0 for input, 1 for output
 *
 * @return          0 on sucess, error code else
 */
static int m32u4_exp_set_direction(struct gpio_chip *chip, unsigned offset, int direction) {
	int status, transferred;
	u8 *data;
	struct m32u4_exp *gpio = gpiochip_get_data(chip);
	gpio->gpio_directions |= (1<<offset);

	data = kzalloc(8, GFP_KERNEL);
	if(!data)
		return -ENOMEM;

	data[offset] = (1<<2)          /* Change pin direction ...*/
		         | (direction<<3); /* ... to in- or output    */

	printk("my_usb_devdrv - data[%d] = 0x%x\n", offset, data[offset]);
	
	status = usb_bulk_msg(gpio->usb_dev, usb_sndbulkpipe(gpio->usb_dev, 1), data, 8, &transferred, 100);
	printk("my_usb_devdrv - Status: %d, Transferred bytes: %d\n", status, transferred);

	/* Free memory */
	kfree(data);
	return 0;
}

static int m32u4_exp_direction_output(struct gpio_chip *chip, unsigned offset, int value)
{
	int status = 0;
	/* Set gpio pin to output */
	status |= m32u4_exp_set_direction(chip, offset, 1);
	/* Set initial value */
	m32u4_exp_set(chip, offset, value);
	return status;
}

static int m32u4_exp_direction_input(struct gpio_chip *chip, unsigned offset) {
	return m32u4_exp_set_direction(chip, offset, 0);
}

static void m32u4_exp_set_mask_bits(struct gpio_chip *chip, u8 mask, u8 bits) {
	struct m32u4_exp *gpio = gpiochip_get_data(chip);
	u8 *data;
	int status, transferred, i;

	data = kzalloc(8, GFP_KERNEL);
	if(!data)
		return;

	for(i=0; i<8; i++) {
		if(mask & (1<<i))
			data[i] = (1 | (3<<2));
		if(bits & (1<<i))
			data[i] |= 2;
	}

	status = usb_bulk_msg(gpio->usb_dev, usb_sndbulkpipe(gpio->usb_dev, 1), data, 8, &transferred, 100);
	printk("my_usb_devdrv - Status: %d, Transferred bytes: %d\n", status, transferred);

	kfree(data);
}

static void m32u4_exp_set(struct gpio_chip *chip, unsigned offset, int value) {
	m32u4_exp_set_mask_bits(chip, BIT(offset), value ? BIT(offset) : 0);
}

static void m32u4_exp_set_multiple(struct gpio_chip *chip, unsigned long *mask, unsigned long *bits) {
	m32u4_exp_set_mask_bits(chip, *mask, *bits);
}

static int m32u4_exp_get_multiple(struct gpio_chip *chip, unsigned long *mask, unsigned long *value) {
	struct m32u4_exp *gpio = gpiochip_get_data(chip);
	u8 *data;
	int status, transferred, i;

	data = kzalloc(8, GFP_KERNEL);
	if(!data)
		return -ENOMEM;

	status = usb_bulk_msg(gpio->usb_dev, usb_rcvbulkpipe(gpio->usb_dev, 0x2), data, 8, &transferred, 100);
	printk("my_usb_devdrv - Status %d, %d Bytes received\n", status, transferred);

	*value = 0;
	for(i=0; i<8; i++) {
		if(*mask & (1<<i))
			*value |= (data[i] << i);
	}

	kfree(data);
	return 0;
}

static int m32u4_exp_get(struct gpio_chip *chip, unsigned gpio_nr) {
	unsigned long mask = 0, bits = 0;

	mask = (1<< gpio_nr);
	m32u4_exp_get_multiple(chip, &mask, &bits);
	return (bits > 0) ? 1 : 0;
}

static const struct gpio_chip template_chip = {
	.label			= "m32u4_exp",
	.owner			= THIS_MODULE,
	.get_direction		= m32u4_exp_get_direction,
	.direction_input	= m32u4_exp_direction_input,
	.direction_output	= m32u4_exp_direction_output,
	.set			= m32u4_exp_set,
	.set_multiple   = m32u4_exp_set_multiple,
	.get            = m32u4_exp_get,
	.get_multiple   = m32u4_exp_get_multiple,
	.base			= -1,
	.ngpio			= 8,
	.can_sleep		= true,
};

static struct usb_device_id usb_dev_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{},
};
MODULE_DEVICE_TABLE(usb, usb_dev_table);

static int my_usb_probe(struct usb_interface *intf, const struct usb_device_id *id) {
	int status,transferred, i;
	struct m32u4_exp *gpio;
	u8 *data;
	printk("my_usb_devdrv - Probe Function\n");
	
	gpio = devm_kzalloc(&intf->dev, sizeof(*gpio), GFP_KERNEL);
	if(!gpio)
		return -ENOMEM;

	gpio->chip = template_chip;
	gpio->usb_dev = interface_to_usbdev(intf);

	usb_set_intfdata(intf, gpio);

	/* Set all pins to inputs at start */
	data = kzalloc(8, GFP_KERNEL);
	if(!data)
		return -ENOMEM;

	for(i=0; i<8; i++)
		data[i] = (1<<2) | (1<<0);
	gpio->gpio_directions = 0;

	status = usb_bulk_msg(gpio->usb_dev, usb_sndbulkpipe(gpio->usb_dev, 1), data, 8, &transferred, 100);
	printk("my_usb_devdrv - Status: %d, Transferred bytes: %d\n", status, transferred);

	status = gpiochip_add_data(&gpio->chip, gpio);
	if(status < 0) 
		printk("my_usb_devdrv - Error adding gpiochip\n");
	kfree(data);
	return status;
}

static void my_usb_disconnect(struct usb_interface *intf) {
	struct m32u4_exp *gpio;
	gpio = usb_get_intfdata(intf);
	gpiochip_remove(&gpio->chip);
	printk("my_usb_devdrv - Disconnect Function\n");
}

static struct usb_driver my_usb_driver = {
	.name = "my_usb_devdrv",
	.id_table = usb_dev_table,
	.probe = my_usb_probe,
	.disconnect = my_usb_disconnect,
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
	int result;
	printk("my_usb_devdrv - Init Function\n");
	result = usb_register(&my_usb_driver);
	if(result) {
		printk("my_usb_devdrv - Error during register!\n");
		return -result;
	}

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
	printk("my_usb_devdrv - Exit Function\n");
	usb_deregister(&my_usb_driver);
}

module_init(my_init);
module_exit(my_exit);


