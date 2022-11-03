#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/types.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("pixelcat");
MODULE_DESCRIPTION("labOS2 kernel module");
MODULE_VERSION("0.01");

static char[] nd_error_msg = "net device not found";

static struct task_struct* get_task_struct_by_pid(const pid_t pid) {
    struct pid* pid_instance = find_get_pid(pid);

    if (pid_instance) return get_pid_task(pid_instance, PIDTYPE_PID);
    else return NULL;
}

static pid_t get_pid_from_task_struct(const struct task_struct* ts) {
    if (ts) return ts->pid;
    else return 0;
}

static struct net_device* get_net_device_by_name(const char* name) {
    return dev_get_by_name(&init_net, name);
}

static char* get_name_from_net_device(const struct net_device* nd) {
    if (nd) return nd->name;
    else return &nd_error_msg;
}

static int __init kmod_init(void) {
    struct task_struct* ts;
    struct net_device* nd;

    printk(KERN_INFO "labos2: module loading\n");

    ts = get_task_struct_by_pid(1);
    printk(KERN_INFO "labos2: test pid is %d\n", get_pid_from_task_struct(ts));

    nd = get_net_device_by_name("enp0s2");
    printk(KERN_INFO "labos2: test nd name is" + get_name_from_net_device(nd) + "\n");

    return 0;
}

static void __exit kmod_exit(void) {
    printk(KERN_INFO "labos2: module unloaded\n");
}
module_init(kmod_init);
module_exit(kmod_exit);
