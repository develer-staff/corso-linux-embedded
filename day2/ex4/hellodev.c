#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVNAME		"hellodev"
#define BUFSIZE		4096

static int minor = 0;
module_param(minor, int, S_IRUGO);

static int major = 0;
static struct class *dev_class = NULL;
static struct device *dev_device = NULL;


static struct file_operations fops = {};

static int __init hellodev_init(void)
{
  int ret;

  pr_info("Loading with minor %d\n", minor);

  major = register_chrdev(major, DEVNAME, &fops);
  if (major < 0) {
    ret = major;
    goto exit;
  }

  dev_class = class_create(DEVNAME);
  if (IS_ERR(dev_class)) {
    ret = PTR_ERR(dev_class);
    goto err_class_create;
  }

  dev_device = device_create(dev_class, NULL, MKDEV(major, minor),
      NULL, DEVNAME "%d", minor);
  if (IS_ERR(dev_device)) {
    ret = PTR_ERR(dev_device);
    goto err_device_create;
  }

  return 0;

err_device_create:
  class_unregister(dev_class);
  class_destroy(dev_class);
err_class_create:
  unregister_chrdev(major, DEVNAME);
exit:
  return ret;
}

static void __exit hellodev_exit(void)
{
  printk(KERN_INFO "Unloading\n");

  device_destroy(dev_class, MKDEV(major, minor));
  class_unregister(dev_class);
  class_destroy(dev_class);
  unregister_chrdev(major, DEVNAME);
}

module_init(hellodev_init);
module_exit(hellodev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello character device");
MODULE_AUTHOR("Me");
