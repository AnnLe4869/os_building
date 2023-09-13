# Bochs setup

See [OSDev Bochs](https://wiki.osdev.org/Bochs). There are very high chance that you need to compile from source in order to use the debugger and the debugger UI. Read the compiling source very careful as you may need to install the `libgtk2.0-dev` even if you may already have GTK 2/3 installed in your machine

When you start the GUI debugger, you may need to refresh the GUI before you can get correct code show. If you were to mess up anything, you can uninstall all you have put by running `make uninstall` and wipe clean the directory by running `make dist-clean`. `make dist-clean` will be needed if you change your `configure` later