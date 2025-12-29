TOOLCHAIN_PREFIX = $(abspath opt/cross/$(TARGET))
export PATH := $(TOOLCHAIN_PREFIX)/bin:$(PATH)


.PHONY: clean_tool_dir toolchain_binutils toolchain_gcc

toolchain: toolchain_binutils toolchain_gcc clean_tool_dir

BINUTIL_BUILD = toolchain/binutils-build-$(BINUTIL_VERSION)

toolchain_binutils:
	mkdir -p toolchain
	cd toolchain && wget $(BINUTILS_URL)
	cd toolchain && tar -xf binutils-$(BINUTIL_VERSION).tar.gz
	mkdir -p $(BINUTIL_BUILD)

	cd $(BINUTIL_BUILD) && ../binutils-$(BINUTIL_VERSION)/configure \
				--prefix="$(TOOLCHAIN_PREFIX)"	\
				--target=$(TARGET) 				\
				--enable-interwork 				\
				--enable-multilib 				\
				--disable-nls 					\
				--disable-werror 				\
				--with-system-zlib
	$(MAKE) -j8 -C $(BINUTIL_BUILD)
	$(MAKE) -C $(BINUTIL_BUILD) install


GCC_BUILD = toolchain/gcc-build-$(GCC_VERSION)

toolchain_gcc:
	mkdir -p toolchain
	cd toolchain && wget $(GCC_URL) && tar -xf gcc-$(GCC_VERSION).tar.gz
	mkdir -p $(GCC_BUILD)

	cd $(GCC_BUILD) && ../gcc-$(GCC_VERSION)/configure \
  			--target=$(TARGET) 				\
			--prefix="$(TOOLCHAIN_PREFIX)"	\
			--disable-nls \
			--enable-languages=c \
			--without-headers \
			--with-gmp=/opt/homebrew \
			--with-mpfr=/opt/homebrew \
			--with-mpc=/opt/homebrew \
			--disable-libsanitizer			
				
	$(MAKE) -j8 -C $(GCC_BUILD) all-gcc all-target-libgcc
	$(MAKE) -C $(GCC_BUILD) install-gcc install-target-libgcc
clean_tool_dir:
	rm -rf toolchain