#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/proc_fs.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A driver for my Atmega32U4 USB device");

#define VENDOR_ID 0x03eb
#define PRODUCT_ID 0x0001

static struct proc_dir_entry *proc_file;
static struct usb_device *usb_dev;

/**
 * @brief Read data out of the buffer
 */
static ssize_t my_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
	char text[32];
	int to_copy, not_copied, delta, status;
	u8 val;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(text));

	/* Read from USB Device */
	status = usb_control_msg_recv(usb_dev, usb_rcvctrlpipe(usb_dev, 0), 0x2, 0xC0, 0, 0, (unsigned char *) &val, 1, 100, GFP_KERNEL);
	if(status < 0) {
		printk("my_usb_devdrv - Error during control message\n");
		return -1;
	}
	sprintf(text, "0x%x\n", val);

	/* Copy data to user */
	not_copied = copy_to_user(user_buffer, text, to_copy);

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

/**
 * @brief Write data to buffer
 */
static ssize_t my_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	char text[255];
	int to_copy, not_copied, delta, status;
	long val;
	u16 seg_val;

	/* Clear text */
	memset(text, 0, sizeof(text));

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(text));

	/* Copy data to user */
	not_copied = copy_from_user(text, user_buffer, to_copy);
	if(0 != kstrtol(text, 0, &val) ) {
		printk("my_usb_devdrv - Error converting input\n");
		return -1;
	}

	seg_val = (u16) val;
	status = usb_control_msg(usb_dev, usb_sndctrlpipe(usb_dev, 0), 0x1, 0x40, seg_val, 0, NULL, 0, 100);
	if(status < 0) {
		printk("my_usb_devdrv - Error during control message\n");
		return -1;
	}

	/* Calculate data */
	delta = to_copy - not_copied;

	return delta;
}

static struct proc_ops fops = {
	.proc_read = my_read,
	.proc_write = my_write,
};


static struct usb_device_id usb_dev_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{},
};
MODULE_DEVICE_TABLE(usb, usb_dev_table);

static int my_usb_probe(struct usb_interface *intf, const struct usb_device_id *id) {
	printk("my_usb_devdrv - Probe Function\n");

	usb_dev = interface_to_usbdev(intf);
	if(usb_dev == NULL) {
		printk("my_usb_devdrv - Error getting device from interface\n");
		return -1;
	}

	proc_file = proc_create("my_usb_dev", 0666, NULL, &fops);
	if(proc_file == NULL) {
		printk("my_usb_devdrv - Error creating /proc/my_usb_dev\n");
		return -ENOMEM;
	}

	return 0;
}

static void my_usb_disconnect(struct usb_interface *intf) {
	proc_remove(proc_file);
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


