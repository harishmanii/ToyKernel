TARGET = i686-elf

export CC=/opt/homebrew/bin/gcc-15
export CXX=/opt/homebrew/bin/g++-15
export LD=/opt/homebrew/bin/gcc-15
export CPPFLAGS=-I/opt/homebrew/include
export LDFLAGS=-L/opt/homebrew/lib
export ASM = nasm
export CFLAGS = -std=c99 -9
export ASM_FLAGS =
export ASM_FLAGS =
export LINKER_FLAGS = 
export LIBS = 
export LINK=

export TARGET_CC = $(TARGET)-gcc
export TARGET_CXX = $(TARGET)-g++
export TARGET_LD = $(TARGET)-gcc
export TARGET_CFLAGS = -std=c99 -g
export TARGET_ASM = nasm
export TARGET_ASM_FLAGS =
export TARGET_LINKER_FLAGS = 
export TARGET_LIBS = 
export TARGET_OBJCOPY = $(TARGET)-objcopy

export BUILD_DIR = $(abspath build)
export IMAGE_DIR = $(BUILD_DIR)/image
export BIN_DIR = $(BUILD_DIR)/bin


BINUTIL_VERSION = 2.45.1
BINUTILS_URL = https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTIL_VERSION).tar.gz

GCC_VERSION = 15.2.0
GCC_URL = https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.gz
