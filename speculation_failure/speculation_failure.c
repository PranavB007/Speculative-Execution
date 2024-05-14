#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <linux/string.h>

SYSCALL_DEFINE2(speculation_failure, pid_t, pid, char __user *, user_str)
{
    struct task_struct *task;
    char k_str[256]; // Kernel space string
    int err = -ESRCH;
    long copy_res;
    size_t i;
    char c;

    // Copy string from user space to kernel space
    copy_res = strncpy_from_user(k_str, user_str, sizeof(k_str));
    if (copy_res < 0)
        return -EFAULT; // Return fault if copy fails

    k_str[sizeof(k_str) - 1] = '\0'; // Ensure null-termination

    // Convert string to lowercase
    for (i = 0; k_str[i]; i++) {
        c = k_str[i];
        if (c >= 'A' && c <= 'Z')
            k_str[i] = c + 32;
    }

    printk(KERN_INFO "Converted string: %s\n", k_str);

    // Find and kill the process
    rcu_read_lock();
    task = find_task_by_vpid(pid);
    if (task) {
        printk(KERN_INFO "Sending SIGKILL to PID: %d\n", pid);
        err = send_sig_info(SIGKILL, SEND_SIG_NOINFO, task);
    } else {
        printk(KERN_INFO "No task found with PID: %d\n", pid);
    }
    rcu_read_unlock();
    return err;
}

