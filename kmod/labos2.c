#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/debugfs.h>

#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("pixelcat");
MODULE_DESCRIPTION("labOS2 kernel module");
MODULE_VERSION("1.0");

//static struct dentry *kmod_root;
//static struct dentry *pid1_task_struct;
//static struct debugfs_blob_wrapper pid1_wrapper;
//static struct task_struct *ts1;

static int __init kmod_init(void) {
    printk(KERN_INFO "labos2: hello world\n");
    return 0;
}

static void __exit kmod_exit(void) {
    printk(KERN_INFO "labos2: bye\n");
}
module_init(kmod_init);
module_exit(kmod_exit);
