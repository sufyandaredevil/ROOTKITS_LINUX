//HelloWorld program as a Linux Kernel Module (LKM)

#include<linux/module.h>
#include<linux/init.h>

/* Initialization routine:
__init is a special identifier (FOR FUNCTIONS) and it tells the linker that it is 
no longer required as soon as the initialization is done so that we can free up that memoy*/
static int __init lkm_init(void){
    pr_info("Hello World of Kernel Programming\n");
    /* pr_info() is similar to printf() that internally uses one of the function available in the kernel called printk
    and in it we're setting the log level to information, and it is stored by default to kernel messages log or what ever config is setup via syslog*/
    return 0;
}

// Exit routine 
// This is omitted if module is compiled into the Kernel which is true for the most part
static void __exit lkm_exit(void){
    pr_info("Wow! You exited early\n");
}

/* when we insert our Loadable Kernel Module (LKM) into the kernel it needs to know what is the first
initialization routine it needs to run, so we call a special function called module_init(),
passing in the function name lkm_init as a parameter */
module_init(lkm_init);

//This is called when we unload/remove the kernel module
module_exit(lkm_exit);

MODULE_AUTHOR("Ahmed Sufyan");
MODULE_DESCRIPTION("A Hello World Module");
MODULE_LICENSE("GPL v2");
/* MODULE_LICENSE() has a significance if we try not to mention `GPL` and instead a proprietary,
then kernel won't allow you to communicate with other modules that are `GPL only`, also ends up tainting the kernel
And still closed source modules(binary modules) are allowed to run but required to mention they're `non GPL` */
