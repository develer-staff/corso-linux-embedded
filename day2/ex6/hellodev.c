#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVNAME		"hellodev"

static int minor = 0;
module_param(minor, uint, S_IRUGO);

static unsigned int bufsize = 4096;
module_param(bufsize, uint, S_IRUGO);

static int major = 0;
static struct class *dev_class = NULL;
static struct device *dev_device = NULL;

static DEFINE_MUTEX(dev_mutex);

static char *dev_buf;

static int dev_open(struct inode *inode, struct file *filp)
{
  pr_info("Open\n");
  return 0;
}

static int dev_close(struct inode *inode, struct file *filp)
{
  pr_info("Close\n");
  return 0;
}

static ssize_t dev_read(struct file *filp, char __user *buf,
    size_t len, loff_t *off)
{
  int ret;

  if (mutex_lock_interruptible(&dev_mutex))
    return -EINTR;

  if (len + *off >= bufsize)
    len = bufsize - *off;

  ret = copy_to_user(buf, dev_buf + *off, len);
  *off += len - ret;

  mutex_unlock(&dev_mutex);

  return len - ret;
}

static ssize_t dev_write(struct file *filp, const char __user *buf,
    size_t len, loff_t *off)
{
  int ret;

  if (mutex_lock_interruptible(&dev_mutex))
    return -EINTR;

  if (len + *off >= bufsize)
    len = bufsize - *off;

  ret = copy_from_user(dev_buf + *off, buf, len);
  *off += len - ret;

  mutex_unlock(&dev_mutex);

  return len - ret;
}

static struct file_operations fops = {
  .open    = dev_open,
  .release = dev_close,
  .read    = dev_read,
  .write   = dev_write,
};

static int __init hellodev_init(void)
{
  int ret;

  pr_info("Loading with minor %d and size %u bytes\n", minor, bufsize);

  // allocate buffer first!
  dev_buf = kzalloc(bufsize, GFP_KERNEL);
  if (unlikely(!dev_buf)) {
    ret = -ENOMEM;
    goto exit;
  }

  major = register_chrdev(major, DEVNAME, &fops);
  if (major < 0) {
    ret = major;
    goto err_register_chrdev;
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
err_register_chrdev:
  kfree(dev_buf);
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
  kfree(dev_buf);
}

module_init(hellodev_init);
module_exit(hellodev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello character device");
MODULE_AUTHOR("Me");
