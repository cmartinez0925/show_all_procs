#define pr_fmt(fmt) "%s::%s():%d: " fmt, KBUILD_MODNAME, __func__, __LINE__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/rcupdate.h>
#include <linux/string.h>
#include <linux/cred.h>
#include <linux/uidgid.h>

MODULE_AUTHOR("Christopher Martinez");
MODULE_VERSION("0.1");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Prints all the processes within the task_list");

#define BUFFER_MAX      256

void show_all_procs(void) {
    struct task_struct *process;
    char buffer[BUFFER_MAX];
    char* header = "     Process     |  TGID  |  PID  |  UID  |  EUID  |";

    pr_info("%s\n", header);
    rcu_read_lock();

    for_each_process(process) {
        get_task_struct(process);

        memset(buffer, 0, BUFFER_MAX);
        snprintf(buffer, BUFFER_MAX, "%-17s|%8d|%7d|%7d|%8d|", 
            process->comm, 
            process->tgid, 
            process->pid, 
            from_kuid(&init_user_ns, task_uid(process)),
            from_kuid(&init_user_ns, task_euid(process))
        );
        pr_info("%s\n", buffer);

        put_task_struct(process);
    }

    rcu_read_unlock();
}

static int __init show_all_procs_init(void) {
    pr_info("Inserted\n");
    show_all_procs();
    return 0;
}

static void __exit show_all_procs_exit(void) {
    pr_info("Removed\n");
}

module_init(show_all_procs_init);
module_exit(show_all_procs_exit);