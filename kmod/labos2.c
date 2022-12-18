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

static struct task_struct* get_task_struct_by_pid(const pid_t pid) {
    struct pid* pid_instance = find_get_pid(pid);
    if (pid_instance) return get_pid_task(pid_instance, PIDTYPE_PID);
    else return NULL;
}

static struct net_device* get_net_device_by_name(const char* name) {
    return dev_get_by_name(&init_net, name);
}

// ------------------------------------------------------------
// INPUT FROM PROCFS
// ------------------------------------------------------------

char* message;
static int mode = 0;

static int allocate_message_memory(void) {
    message = kmalloc(50*sizeof(char), GFP_KERNEL);
    if (message) return 0;
    return -1;
}

static int set_mode(void) {
    if (!message[0]) return -1;
    switch (message[0]) {
        case 'p':
            mode = 1;
            return 0;
        case 'd':
            mode = 2;
            return 0;
        default:
            mode = 0;
            return -1;
    }
}

static void set_task_struct(void) {
    long num_input;
    char* useful_message = &message[1];

    printk(KERN_INFO "labOS2: Retrieving task_struct with pid = %s...\n", useful_message);
    if (kstrtoul(useful_message, 10, &num_input) != 0) {
        printk(KERN_WARNING "labOS2: Invalid input!\n");
        return;
    }

    ts = get_task_struct_by_pid(num_input);
    printk(KERN_INFO "labOS2: Done.\n");
}

static void set_net_device(void) {
    char* useful_message = &message[1];

    printk(KERN_INFO "labOS2: Retrieving net_device with name = %s...\n", useful_message);
    
    nd = get_net_device_by_name(useful_message);
    printk(KERN_INFO "labOS2: Done.\n");
}


static void handle_input(void) {
    if (set_mode() != 0) {
        printk(KERN_WARNING "labOS2: Invalid input!\n");
        return;
    }

    printk(KERN_INFO "labOS2: Set output mode to %d\n", mode);

    switch (mode) {
        case 1:
            set_task_struct();
            break;
        case 2:
            set_net_device();
            break;
    }
}

static ssize_t proc_write(struct file* filp, const char* buf, size_t count, loff_t* offp) {
    if (copy_from_user(message, buf, count) != 0) {
        printk(KERN_WARNING "labOS2: Couldn't read input!\n");
        return count;
    }
    printk(KERN_INFO "labOS2: Retrieved message: %s\n", message);
    handle_input();
    return count;
}

// ------------------------------------------------------------
// OUTPUT TO PROCFS
// ------------------------------------------------------------

static int output_task_struct(struct seq_file* m, void* v) {
    printk(KERN_INFO "labOS2: Trying to display task_struct info...\n");
    if (ts) {
        seq_printf(m, "task_struct:\n"                              );
        seq_printf(m, "pid          = %d\n",        ts->pid         );
        seq_printf(m, "tgid         = %d\n",        ts->tgid        );
        seq_printf(m, "state        = %ld\n",       ts->state       );
        seq_printf(m, "flags        = %d\n",        ts->flags       );
        printk(KERN_INFO "labOS2: task_struct info displayed successfully.\n");
    }
    else {
        seq_printf(m, "task_struct not found!\n"                    );
        printk(KERN_WARNING "labOS2: Couldn't display task_struct!\n");
    }
    return 0;
}

static int output_net_device(struct seq_file* m, void* v) {
    printk(KERN_INFO "labOS2: Trying to display net_device info...\n");
    if (nd) {
        seq_printf(m, "net_device:\n"                               );
        seq_printf(m, "name         = %s\n",        nd->name        );
        seq_printf(m, "mem_start    = %ld\n",       nd->mem_start   );
        seq_printf(m, "mem_end      = %ld\n",       nd->mem_end     );
        seq_printf(m, "base_addr    = %ld\n",       nd->base_addr   );
        seq_printf(m, "irq          = %d\n",        nd->irq         );
        printk(KERN_INFO "labOS2: task_struct info displayed successfully.\n");
    }
    else {
        seq_printf(m, "net_device not found!\n"                     );
        printk(KERN_WARNING "labOS2: Couldn't display net_device!\n");
    }
    return 0;
}

static int output(struct seq_file* m, void* v) {
    printk(KERN_INFO "labOS2: Output function called.\n");
    switch (mode) {
        case 1:
            return output_task_struct(m, v);
        case 2:
            return output_net_device(m, v);
        default:
            seq_printf(m, "Input not set!\n");
            printk(KERN_WARNING "labOS2: No data to output!\n");
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
    .proc_open      = proc_open,
    .proc_read      = seq_read,
    .proc_write     = proc_write,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
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
    printk(KERN_INFO "labOS2: Module loading...\n");

    printk(KERN_INFO "labOS2: Attempting to create procfs file...\n");
    if (create_proc_file() != 0) {
        printk(KERN_ALERT "labOS2: Couldn't make /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "labOS2: /proc/%s created\n", PROCFS_NAME);	

    printk(KERN_INFO "labOS2: Attempting to allocate memory for incoming messages...\n");
    if (allocate_message_memory() != 0) {
        printk(KERN_ALERT "labOS2: Couldn't allocate memory\n");
        return -1;
    }

    printk(KERN_INFO "labOS2: Waiting for input\n");
    return 0;
}

static void __exit kmod_exit(void) {
    printk(KERN_INFO "labOS2: Unloading module...\n");

    kfree(message);
	printk(KERN_INFO "labOS2: Allocated memory freed\n");
    remove_proc_entry(PROCFS_NAME, NULL);
	printk(KERN_INFO "labOS2: /proc/%s removed\n", PROCFS_NAME);

    printk(KERN_INFO "labOS2: Module unloaded\n");
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
