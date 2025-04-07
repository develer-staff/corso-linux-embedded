#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static char *whom = "World";
module_param(whom, charp, 0644);
MODULE_PARM_DESC(whom, "Hello message");

static int howmany = 1;
module_param(howmany, int, 0644);
MODULE_PARM_DESC(howmany, "Num greetings");

static int __init hello_init(void) {
  for (int i = 0; i < howmany; ++i) {
    pr_info("Hello %s\n", whom);
  }
  return 0;
}

static void __exit hello_exit(void) {
  pr_info("Bye %s\n", whom);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello module");
MODULE_AUTHOR("Me");
