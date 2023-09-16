
TOOLCHAIN_PREFIX = $(abspath toolchain/$(TARGET))
export PATH := $(TOOLCHAIN_PREFIX)/bin:$(PATH)

toolchain: toolchain_binutils toolchain_gcc

BINUTILS_SRC = toolchain/binutils-$(BINUTILS_VERSION)
BINUTILS_BUILD = toolchain/binutils-build-$(BINUTILS_VERSION)

toolchain_binutils: $(BINUTILS_SRC).tar.xz
	cd toolchain && tar -xf binutils-$(BINUTILS_VERSION).tar.xz
	mkdir $(BINUTILS_BUILD)
	# --prefix: where to install all packages
	# --target: system target to compile
	# --with-sysroot: tell linker and compiler where to find the target system library - in our case, leave it blank tell not to build with sysroot support+
	#				: helpful when need to do cross-os development
	# --disable-nls: disable native langauge support - means error only in english
	# --disable-werror: so that compiling won't stop when encounter warning
	cd $(BINUTILS_BUILD) && ../binutils-$(BINUTILS_VERSION)/configure \
		--prefix="$(TOOLCHAIN_PREFIX)"	\
		--target=$(TARGET)				\
		--with-sysroot					\
		--disable-nls					\
		--disable-werror
	$(MAKE) -j8 -C $(BINUTILS_BUILD)
	$(MAKE) -C $(BINUTILS_BUILD) install

$(BINUTILS_SRC).tar.xz:
	mkdir -p toolchain 
	cd toolchain && wget $(BINUTILS_URL)


GCC_SRC = toolchain/gcc-$(GCC_VERSION)
GCC_BUILD = toolchain/gcc-build-$(GCC_VERSION)

toolchain_gcc: toolchain_binutils $(GCC_SRC).tar.xz
	cd toolchain && tar -xf gcc-$(GCC_VERSION).tar.xz
	mkdir $(GCC_BUILD)
	# --without-headers: build without relying on host system library
	cd $(GCC_BUILD) && ../gcc-$(GCC_VERSION)/configure \
		--prefix="$(TOOLCHAIN_PREFIX)" 	\
		--target=$(TARGET)				\
		--disable-nls					\
		--enable-languages=c,c++		\
		--without-headers
	$(MAKE) -j8 -C $(GCC_BUILD) all-gcc all-target-libgcc
	$(MAKE) -C $(GCC_BUILD) install-gcc install-target-libgcc
	
$(GCC_SRC).tar.xz:
	mkdir -p toolchain
	cd toolchain && wget $(GCC_URL)

#
# Clean
#
clean-toolchain:
	rm -rf $(GCC_BUILD) $(GCC_SRC) $(BINUTILS_BUILD) $(BINUTILS_SRC)

clean-toolchain-all:
	rm -rf toolchain/*

.PHONY:  toolchain toolchain_binutils toolchain_gcc clean-toolchain clean-toolchain-all