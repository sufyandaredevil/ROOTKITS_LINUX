obj-m += helloworld.o
# obj-m (object module) to compile `helloworld.c` as a module with the name `helloworld.o` 
# (even though the extension of the source file is `helloworld.c`)

modules:
		@$(MAKE) -C $(KERNEL_ROOT) M=$(shell pwd) modules
# when we call `make`, make sure to pass the location of kernel root directory in the variable `KERNEL_ROOT`
# then compile the modules located in the current dir (`M=$(shell pwd)`)

clean:
		@$(MAKE) -C $(KERNEL_ROOT) M=$(shell pwd) clean
