OS : LINUX (UBUNTU 18.04 x86_64) (64 Bit)  
OS KERNEL VERSION: Linux 4.16.7

### Resources:
  - [Linux Kernel Map](./LKM.pdf)
(For interactive webview click [here](https://makelinux.github.io/kernel/map/))
  - To browse the Linux Kernel Source Code:
    - [LXR: Linux Cross Reference Browser](https://lxr.sourceforge.io/en/index.php)
    - [GNU Global](https://www.gnu.org/software/global/)
    - [Elixir](https://elixir.bootlin.com/linux/latest/source)
    - Cscope & Ctags (we'll use this)

### Linux Tools & Kernel Setup:
  - `sudo apt install build-essential fakeroot bison flex ccache kernel-package libncurses5-dev libssl-dev libelf-dev cscope ctags`
  - Kernel modules that are used to develop rootkits needs to be compiled against very specific version of the kernel we wanna run it on. So we make use of [Linux Kernel Archive](https://linuxkernel.org) to download the latest kernel
    > **NOTE:** As per the time of taking notes from the course material the instructor uses the [Linux Kernel version 4.16.7](https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.16.7.tar.xz) with the custom suffix **-rootkit** to differentiate it with other kernels, and so did I
  - Download the latest kernel and in our case:
    - `wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.16.7.tar.xz`
  - Now to compile the kernel:
    - `mkdir linux/` & untar the kernel in it:
      - `tar xf <XYZ.tar.xz>`
  - Get config file from the already installed linux machine to use it as a baseline for our new kernel:
    - `cp /boot/config-<uname -r> <new_kernel_extracted_dir>/.config`
    - The .config files consists of all options which were used to create the kernel running on the current linux machine so we need to migrate some of the options to the newer version of the kernel we're about to install
  - Automatically Updating Kernel Configuration with Default Values of the current linux kernel for Transitioning to a New Kernel Version ([for more info](./whats_that/linux_kernel_config.md)):
    - `yes '' | make olddefconfig`
  - In case we wanna make any changes to the .config file which we imported from the current kernel we can use `make nconfig` or the old `make menuconfig` automatically takes in the .config file for a GUI experience before compiling the newer kernel
  - Clean object file and other misc: `make clean`
  - Make sure we have enough CPUs available to have a faster compilation time (`nproc` to view no of cpus)
  - Then `make -j4 deb-pkg LOCALVERSION=-rootkit` ([for more info](./whats_that/make_in_kernel_suffix.md)). Here's where the compilation happens...
  - After compilation if we go up one directory you'll see **.deb** packages available for installation, so from here we need to first install the image and the one that doesn't have the **-dbg** string, so in this case:
    - `sudo dpkg -i linux-image-<kernel_version>-rootkit_<kernel_version>-rootkit-1_amd64.deb`
  - Ater installation inside the **/boot** directory you'll find:
    - **config-<kernel_version>-rootkit**
    - **initrd.img-<kernel_version>-rootkit**
    - **System.map-<kernel_version>**
    - **vmlinuz-<kernel_version>-rootkit**
  - Now to install linux headers, followed by linux libc-dev:
    - `sudo dpkg -i linux-headers-<kernel_version>-rootkit_<kernel_version>-rootkit-1_amd64.deb`
    - `sudo dpkg -i linux-libc-dev<kernel_version>-rootkit_<kernel_version>-rootkit-1_amd64.deb`
  - Comment out the following in the file: `/etc/defaut/grub` ([For more info](./whats_that/grub_hidden_timeout.md))
      - **GRUB_HIDDEN_TIMEOUT**
      - **GRUB_HIDDEN_TIMEOUT_QUIET**
  - Update grub config file:
    - `sudo update-grub`
  - **`reboot`**
  - Now in the GRUB Boot Menu we select `Advanced options for Ubuntu` and see that the default selected kernel(listing shown in order where 1st one being the default) is the one that we've installed recently so far (having the suffix '**-rootkit**'). We press **[ENTER]** to proceed
  - After logging in verify with `uname -r` to see if the <kernel_version>-rootkit show up. And in this case it was **4.16.7-rootkit** letting us know the installation was a success
  - Also make sure to set `sudo dmesg -n 7`. When you set the kernel message log level to 7, all levels of log messages will be shown. Linux maintains a kernel ring buffer, which temporarily holds kernel log messages. These messages can be accessed using the `dmesg` command without any arguments, so on running it you run all log messages, including debug messages, will be added to this buffer The log levels in the kernel defined as follows (in ascending order of severity):
    1. **KERN_EMERG**: System is unusable (highest priority level).
    2. **KERN_ALERT**: Action must be taken immediately.
    3. **KERN_CRIT**: Critical conditions.
    4. **KERN_ERR**: Error conditions.
    5. **KERN_WARNING**: Warning conditions.
    6. **KERN_NOTICE**: Normal, but significant events.
    7. **KERN_INFO**: Informational messages.
    8. **KERN_DEBUG**: Debug-level messages (lowest priority level)

### Creating [Hello World](./HelloWorld_LKM/helloworld.c) program as a Linux Kernel Module (LKM):
  - > **helloworld.c**
    > ```c
    > //HelloWorld program as a Linux Kernel Module (LKM)
    > 
    > #include<linux/module.h>
    > #include<linux/init.h>
    > 
    > /* Initialization routine:
    > __init is a special identifier (FOR FUNCTIONS) and it tells the linker that it is 
    > no longer required as soon as the initialization is done so that we can free up that memoy*/
    > static int __init lkm_init(void){
    >     pr_info("Hello World of Kernel Programming\n");
    >     /* pr_info() is similar to printf() that internally uses one of the function available in the kernel called printk
    >     and in it we're setting the log level to information, and it is stored by default to kernel messages log or what ever config is setup via syslog
    >     Also make sure to add a `\n` after printing the message because it will save into the buffer instead of storing in the log file*/
    >     return 0;
    > }
    > 
    > // Exit routine 
    > // This is omitted if module is compiled into the Kernel which is true for the most part
    > static void __exit lkm_exit(void){
    >     pr_info("Wow! You exited early\n");
    > }
    > 
    > /* when we insert our Loadable Kernel Module (LKM) into the kernel it needs to know what is the first
    > initialization routine it needs to run, so we call a special function called module_init(),
    > passing in the function name lkm_init as a parameter */
    > module_init(lkm_init);
    > 
    > //This is called when we unload/remove the kernel module
    > module_exit(lkm_exit);
    > 
    > MODULE_AUTHOR("Ahmed Sufyan");
    > MODULE_DESCRIPTION("A Hello World Module");
    > MODULE_LICENSE("GPL v2");
    > /* MODULE_LICENSE() has a significance if we try not to mention `GPL` and instead a proprietary,
    > then kernel won't allow you to communicate with other modules that are `GPL only`, also ends up tainting the kernel
    > And still closed source modules(binary modules) are allowed to run but required to mention they're `non GPL` */
    > ```
  - > **Makefile**
    >```Makefile
    > obj-m += helloworld.o
    > # obj-m (object module) to compile `helloworld.c` as a module with the name `helloworld.o` 
    > # (even though the extension of the source file is `helloworld.c`)
    > 
    > modules:
    > 		@$(MAKE) -C $(KERNEL_ROOT) M=$(shell pwd) modules
    > # when we call `make`, make sure to pass the location of kernel root directory in the variable `KERNEL_ROOT`
    > # then compile the modules located in the current dir (`M=$(shell pwd)`)
    > 
    > clean:
    > 		@$(MAKE) -C $(KERNEL_ROOT) M=$(shell pwd) clean
    >```
    >
    > **NOTE:**
    > - When we are compiling a kernel module we need to refer to the kernel sources (stuffs that we downloaded [earlier](#tools-to-install-and-environment-setup))
    > - For the latest ones you need a source tree that's prepared for module compilation or preferably if you've compiled then entire kernel
    > - For our case we have the kernel source itself so we'll make use of it.
  - **To compile the HelloWorld LKM:**
    - Using the **Makefile** we pass in the directory of the linux kernel source (in our case: **/home/$(whoami)/linux/linux-4.16.7**) as a parameter to `KERNEL_ROOT` variable:  
      - `KERNELROOT=/home/<whoami>/linux/linux-4.16.7 make`
    - From the file generated called `helloworld.ko` we can get some info using `file` or `modinfo` command
  - **To insert HelloWorld LKM into the kernel:**
    - `sudo insmod helloworld.ko`
  - **Check if the module is running:**
    - `lsmod`
  - **What about the string given as a param for pr_info() ?:**
    - `dmesg`
    - Also present in `/var/log/kern.log` (that being the default store for kernel log messages)
  - **To remove HelloWorld LKM from the kernel:**
    - `sudo rmmod helloworld`
  - **To clean the generated misc from source:**
      - `KERNELROOT=/home/<whoami>/linux/linux-4.16.7 make clean`

### MISC Commands:
  - Insert a kernel module:
    - `sudo insmod <kernel_module>.ko [param_name=param_value]`
  - Print kernel log level config:
    - `cat /proc/sys/kernel/printk`
  - List all modules loaded:
    - `ls /sys/module`
      - **NOTE:** When setting up a param in an LKM using the **module_param(value, type, permission)** function we need to mention set of permissions (as the third argument) to make those params accessible to certain users and also create an entry in `/sys/module`
  - Clear the kernel ring buffer
    - `sudo dmesg -C`
  - Change Console log level (Console being the screen seen when pressing <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>F1</kbd>, not referring the GUI Terminal that you'd normally use):
    - `sudo dmesg -n <log_level_number>`
      - example: `sudo dmesg -n 7`
      - Refer these ([1](./Screenshot%20(23).png), [2](Screenshot%20(24).png)) for more
  - [printk formats](https://www.kernel.org/doc/html/latest/core-api/printk-formats.html)

### Cscope & Ctags:
  - What do they do?
    - Cscope: cscope is a tool that generates a cross-reference database for C and C++ source code. It creates an index of symbols, functions, definitions, and references within the codebase. When you run make cscope inside the Linux kernel source tree, it generates a cscope.out file containing the cross-reference database. Once the cscope.out file is generated, you can use the cscope command-line tool or GUI frontend (if available) to perform searches for symbols, functions, or references. It allows you to quickly locate where specific functions are defined, referenced, or called, making it easier to navigate the source code.
  - Ctags: ctags is another tool for generating an index of symbols in source code files, specifically for C, C++, and related languages. When you run make ctags inside the Linux kernel source tree, it generates a tags file containing the index of symbols. The tags file generated by ctags is used by text editors and IDEs that support tag-based code navigation. Editors like Vim and Emacs can use the tags file to jump directly to the definition of a symbol or function. The tags file helps in quickly navigating the codebase and can be used to browse the kernel source efficiently.
  - `make tags` and `make cscope` were ran inside the linux kernel source itself (**/home/whoami/linux/linux-4.16.7/**)
  - `cscope -d`
    - **-d** option creates a cross-reference db with symbol information for global definitions only such as global functions, global variables, and macros
    - **Without the -d** includes both global and local symbols

### THINGS TO REMEMBER:
  - When passing in parameters like in the following case (for source code, refer this file: `helloworld-params.c` <--Only available locally):
    ```sh
    sudo insmod helloworld-params.ko counter=100 message="Hello World" `
    ```
    - When we print the logs using `dmesg` you'll find the message `unknown parameter 'World' ignored`
    - The reason is that the shell strips the string World. So to make the shell consider it we need to wrap it around single and double quotes:
      - `sudo insmod helloworld-params.ko counter=100 message='"Hello World"'`
  - **/proc** and **/sys** directory:
    - `/proc` directory **(procfs)** is a virtual filesystem created in RAM that was originally created to hold process information but later since it turned out to be a dumping ground where a lot of non-process information was also present that had no structure or hierarchy
    - Later an alternative `/sys` directory **(sysfs)** was created which maps kernel subsystems, device drivers and their hierarchy, so every kobject (kernel object) has a mapping in sysfs
    - Also **/proc** still co-exist with **/sys** and haven't been replaced
  - Sometimes `tast_struct` is also called as the **process descriptor** (`task_struct` is the data structure that defines task itself in the kernel space (task being a thread in the context of kernel space, and there's no existence of process in the kernel space and only available in the user space))
    - For more about `task_struct` data structure you can refer it from the kernel source code and in our case it is present in: `/home/whoami/linux/linux-4.16.7/include/linux/sched.h`
