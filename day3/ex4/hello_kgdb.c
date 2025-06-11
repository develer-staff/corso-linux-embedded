// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static struct task_struct *kthread;

static int hello_kgdb_thread_routine(void *data)
{
	unsigned long i = 0;

	while(1) {
		i++;
		usleep_range(1000, 2000);

		pr_debug("I'm awake ! (loop %ld)\n", i);

		if (kthread_should_stop())
			break;
	}

	return 0;
}

static int hello_kgdb_init(void)
{
	kthread = kthread_create(hello_kgdb_thread_routine, NULL, "hello_kgdb");
	if (!kthread)
		return -EINVAL;

	wake_up_process(kthread);

	return 0;
}

static void hello_kgdb_exit(void)
{
	kthread_stop(kthread);
}

module_init(hello_kgdb_init);
module_exit(hello_kgdb_exit);
MODULE_LICENSE("GPL");