# CSE103-OS Makefile
# Target: x86 (i686, 32-bit protected mode), GRUB Multiboot, QEMU

CC ?= $(shell command -v i686-elf-gcc 2>/dev/null || command -v i686-linux-gnu-gcc 2>/dev/null || echo gcc)
LD ?= $(shell command -v i686-elf-ld 2>/dev/null || command -v i686-linux-gnu-ld 2>/dev/null || echo ld)
QEMU ?= $(shell command -v qemu-system-i386 2>/dev/null || echo qemu-system-i386)
GRUB_MKRESCUE ?= $(shell command -v grub-mkrescue 2>/dev/null || command -v grub2-mkrescue 2>/dev/null || echo grub-mkrescue)

CFLAGS := -std=c11 -m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -Iinclude
LDFLAGS := -m elf_i386 -T linker.ld -nostdlib

BUILD_DIR := build
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
OS_ISO := $(BUILD_DIR)/cse103-os.iso

# Source files
SRCS := $(wildcard src/kernel/*.c) \
        $(wildcard src/arch/x86/*.c) \
        $(wildcard src/drivers/*.c) \
        $(wildcard src/mm/*.c) \
        $(wildcard src/lib/*.c) \
        $(wildcard src/shell/*.c)

# Object files
OBJS := $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all iso run test debug clean

all: $(KERNEL_ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/kernel
	mkdir -p $(BUILD_DIR)/arch/x86
	mkdir -p $(BUILD_DIR)/drivers
	mkdir -p $(BUILD_DIR)/mm
	mkdir -p $(BUILD_DIR)/lib
	mkdir -p $(BUILD_DIR)/shell

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@

iso: $(OS_ISO)

$(OS_ISO): $(KERNEL_ELF) boot/grub/grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL_ELF) isodir/boot/kernel.elf
	cp boot/grub/grub.cfg isodir/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $(OS_ISO) isodir
	rm -rf isodir

run: $(OS_ISO)
	@echo "Starting CSE103-OS interactive emulator..."
	$(QEMU) -cdrom $(OS_ISO) -serial stdio

run-kernel: $(KERNEL_ELF)
	@echo "Starting CSE103-OS direct kernel boot..."
	$(QEMU) -kernel $(KERNEL_ELF) -serial stdio

test: $(OS_ISO)
	@echo "Running CSE103-OS automated headless test harness..."
	@python3 -c "import subprocess, sys, time; \
p = subprocess.Popen(['$(QEMU)', '-kernel', '$(KERNEL_ELF)', '-display', 'none', '-serial', 'stdio'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True); \
time.sleep(0.5); \
p.stdin.write('help\nmeminfo\nuptime\ntest\nhalt\n'); \
p.stdin.flush(); \
out, _ = p.communicate(timeout=6); \
print(out); \
assert '[BOOT] CSE103-OS kernel started' in out, 'Missing boot marker'; \
assert 'CSE103-OS>' in out, 'Missing shell prompt'; \
assert 'meminfo' in out or 'Physical Memory' in out, 'Missing meminfo output'; \
assert '[PASS]' in out, 'Kernel tests failed'; \
print('=== ALL HEADLESS CHECKS PASSED (EXIT 0) ===')"

debug: $(KERNEL_ELF)
	@echo "Starting QEMU waiting for GDB on :1234..."
	@echo "Connect using: gdb $(KERNEL_ELF) -ex 'target remote localhost:1234'"
	$(QEMU) -kernel $(KERNEL_ELF) -s -S -serial stdio

clean:
	rm -rf $(BUILD_DIR) isodir *.iso *.elf
