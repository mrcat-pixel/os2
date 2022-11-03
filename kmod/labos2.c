#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/types.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("pixelcat");
MODULE_DESCRIPTION("labOS2 kernel module");
MODULE_VERSION("1.0");

static struct task_struct* get_task_struct_by_pid(pid_t pid) {
    return get_pid_task(find_get_pid(pid), PIDTYPE_PID)
}

static int __init kmod_init(void) {
    printk(KERN_INFO "labos2: module loading\n");
    struct task_struct* ts = get_task_struct_by_pid(1);
    printk(KERN_INFO "labos2: test pid is %d\n", ts->pid);
    return 0;
}

static void __exit kmod_exit(void) {
    printk(KERN_INFO "labos2: module unloaded\n");
}
module_init(kmod_init);
module_exit(kmod_exit);
