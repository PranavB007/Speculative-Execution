#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched/signal.h>

SYSCALL_DEFINE1(speculation_successful, pid_t, pid)
{
    struct task_struct *task;
    int err = -ESRCH;
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
