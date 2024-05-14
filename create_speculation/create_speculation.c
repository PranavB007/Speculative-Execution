#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE2(create_speculation, pid_t, pid, char __user *, user_str)
{
    struct task_struct *task;
    char *kernel_str;
    long copied;
    size_t len;
    int i;
    printk(KERN_INFO "strtoupper syscall called with pid: %d\n", pid);
    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    rcu_read_unlock();
    if (!task) {
        printk(KERN_INFO "No process found with PID: %d\n", pid);
        return -ESRCH;
    }
    printk(KERN_INFO "Process found, starting to process string from user space\n");
    if (!access_ok(user_str, 1)) {
        printk(KERN_INFO "Invalid user space address provided\n");
        return -EFAULT;
    }
    len = strnlen_user(user_str, 100);
    if (len > 100 || len == 0) {
        printk(KERN_INFO "Invalid string length: %zu\n", len);
        return -EINVAL;
    }
    kernel_str = kmalloc(len, GFP_KERNEL);
    if (!kernel_str) {
        printk(KERN_INFO "Out of memory when allocating kernel_str\n");
        return -ENOMEM;
    }
    copied = strncpy_from_user(kernel_str, user_str, len);
    if (copied != len - 1) {
        printk(KERN_INFO "Failed to copy string from user. Expected %zu, got %ld\n", len-1, copied);
        kfree(kernel_str);
        return -EFAULT;
    }
    printk(KERN_INFO "String copied from user space, converting to uppercase\n");
    for (i = 0; i < len - 1; i++) {
        if (kernel_str[i] >= 'a' && kernel_str[i] <= 'z') {
            kernel_str[i] -= 32;
        }
    }
    if (copy_to_user(user_str, kernel_str, len)) {
        printk(KERN_INFO "Failed to copy back the uppercase string to user space\n");
        kfree(kernel_str);
        return -EFAULT;
    }
    printk(KERN_INFO "String converted to uppercase and copied back to user space successfully\n");
    kfree(kernel_str);
    return 0;
}

