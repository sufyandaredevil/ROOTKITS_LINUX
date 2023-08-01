OS : LINUX (UBUNTU 18.04 amd64)  
OS ARCHITECTURE: x86_64 (64 Bit)  
OS KERNEL VERSION (on start): Linux 4.16.7

### External Resources:
  - [Linux Kernel Map](./LKM.pdf)
(For interactive webview click [here](https://makelinux.github.io/kernel/map/))

### Tools to install and Environment Setup:
  - `sudo apt install build-essential fakeroot bison flex ccache kernel-package libncurses5-dev libssl-dev libelf-dev`
  - Kernel modules that are used to develop rootkits needs to be compiled against very specific version of the kernel we wanna run it on. So we make use of [Linux Kernel Archive](https://linxukernel.org) to down the latest kernel
    > **NOTE:** As per the time of taking notes from the course material the instructor uses the [Linux Kernel version 4.16.7](https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.16.7.tar.xz) with the custom suffix **-rootkit** to differentiate it other kernels, and so did I.
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
  - In case we wanna make any changes to the .config file which we imported from the current kernel we can use `make nconfig` or the old menuconfig before compiling the newer kernel
  - Clean object file and other misc: `make clean`
  - Make sure we have enough CPUs available to have a faster compilation time (`nproc` to view no of cpus)
  - Then `make -j4 deb-pkg LOCALVERSION=-rootkit` ([for more info](./whats_that/make_in_kernel_suffix.md)). Here's where the compilation happens...
  - After compilation if we go up one directory you'll see **.deb** packages available for installation, so from here we need to first install the image and the one that doesn't have the **-dbg** string, so in this case:
    - `sudo dpkg -i linux-image-<kernel_version>-rootkit_<kernel_version>-rootkit-1_amd64.deb`
  - Ater installation inside the **/boot** directory you'll find **vmlinuz-<kernel_version>-rootkit**, **System.map-<kernel_version>**, **initrd.img-<kernel_version>-rootkit**, **config-<kernel_version>-rootkit**
  - Now to install linux headers, followed by linux libc-dev:
    - `sudo dpkg -i linux-headers-<kernel_version>-rootkit_<kernel_version>-rootkit-1_amd64.deb`
    - `sudo dpkg -i linux-libc-dev<kernel_version>-rootkit_<kernel_version>-rootkit-1_amd64.deb`
  - Comment out the following in the file: `/etc/defaut/grub` ([For more info](./whats_that/grub_hidden_timeout.md))
      - **GRUB_HIDDEN_TIMEOUT**
      - **GRUB_HIDDEN_TIMEOUT_QUIET**
  - Update grub config file:
    - `sudo update-grub`
  - **`reboot`**
  - Now in the GRUB Boot Menu we select `Advanced options for Ubuntu` and see that the default selected kernel(indicated with a `*` symbol) is the one that we've installed recently so far (having the suffix '**-rootkit**'). We press **[ENTER]** to proceed
  - After logging in verify with `uname -r` to see if the <kernel_version>-rootkit show up. And in this case it was **4.16.7-rootkit** letting us know the installation was a success
