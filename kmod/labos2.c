#include <linux/init.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/types.h>
#include <asm/uaccess.h>

// ------------------------------------------------------------
// GETTING STRUCTURES
// ------------------------------------------------------------

static struct task_struct* ts;
static struct net_device* nd;
static int mode = 0;

static struct task_struct* get_task_struct_by_pid(const pid_t pid) {
    struct pid* pid_instance = find_get_pid(pid);
    if (pid_instance) return get_pid_task(pid_instance, PIDTYPE_PID);
    else return NULL;
}

static struct net_device* get_net_device_by_name(const char* name) {
    return dev_get_by_name(&init_net, name);
}

// ------------------------------------------------------------
// OUTPUT TO PROCFS
// ------------------------------------------------------------

static int output_task_struct(struct seq_file* m, void* v) {
    printk(KERN_INFO "labOS2: trying to display task struct info\n");
    if (ts) {
        seq_printf(m, "task_struct:\n");
        seq_printf(m, "pid   = %d\n",  ts->pid);
        seq_printf(m, "tgid  = %d\n",  ts->tgid);
        seq_printf(m, "state = %ld\n", ts->state);
        seq_printf(m, "flags = %d\n",  ts->flags);
        return 0;
    }
    else {
        printk(KERN_INFO "labOS2: couldn't display task struct\n");
        seq_printf(m, "task_struct not found!\n");
        return 0;
    }
}

static int output_net_device(struct seq_file* m, void* v) {
    printk(KERN_INFO "labOS2: trying to display net device info\n");
    if (nd) {
        seq_printf(m, "net_device:\n");
        seq_printf(m, "name         = %s\n",  nd->name);
        seq_printf(m, "mem_start    = %ld\n", nd->mem_start);
        seq_printf(m, "mem_end      = %ld\n", nd->mem_end);
        seq_printf(m, "base_addr    = %ld\n", nd->base_addr);
        seq_printf(m, "irq          = %d\n",  nd->irq);
        return 0;
    }
    else {
        printk(KERN_INFO "labOS2: couldn't display net device\n");
        seq_printf(m, "error getting net_device!\n");
        return 0;
    }
}

static int output(struct seq_file* m, void* v) {
    switch (mode) {
        case 1:
            return output_task_struct(m, v);
        case 2:
            return output_net_device(m, v);
        default:
            seq_printf(m, "Input not set!\n");
            return 0;
    }
}

static int proc_open(struct inode* inode, struct file* file) {
    return single_open(file, output, NULL);
}

// ------------------------------------------------------------
// PROCFS INIT
// ------------------------------------------------------------

#define PROCFS_NAME "labOS2"

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int create_proc_file(void) {
    struct proc_dir_entry *proc_file;
    proc_file = proc_create(PROCFS_NAME, 0, NULL, &proc_fops);
    if (proc_file == NULL) {
        remove_proc_entry(PROCFS_NAME, NULL);
        return -1;
    }
    return 0;
}

// ------------------------------------------------------------
// INIT AND END
// ------------------------------------------------------------

static int __init kmod_init(void) {
    printk(KERN_INFO "labOS2: module loading\n");

    printk(KERN_INFO "labOS2: attempting to create procfs file\n");
    if (create_proc_file() != 0) {
        printk(KERN_ALERT "labOS2: couldn't make /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "labOS2: /proc/%s created\n", PROCFS_NAME);	
    printk(KERN_INFO "labOS2: waiting for input\n");

    mode = 2;

    ts = get_task_struct_by_pid(1);
    nd = get_net_device_by_name("lo");

    return 0;
}

static void __exit kmod_exit(void) {
    remove_proc_entry(PROCFS_NAME, NULL);
	printk(KERN_INFO "labOS2: /proc/%s removed\n", PROCFS_NAME);
    printk(KERN_INFO "labOS2: module unloaded\n");
}

// ------------------------------------------------------------
// INFO
// ------------------------------------------------------------

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("pixelcat");
MODULE_DESCRIPTION("labOS2 kernel module");
MODULE_VERSION("0.01");

module_init(kmod_init);
module_exit(kmod_exit);
