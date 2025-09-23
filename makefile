# ====================================================================
#                      Basic Variable Definitions
# ====================================================================
BUILD_DIR := build
SRC_DIR := src

# Kernel entry point address
ENTRYPOINT := 0x10000

# Compilers and linkers
CC := gcc
LD := ld
ASM := nasm

# ====================================================================
#                   Compilation and Linker Options
# ====================================================================
# C compiler options
CFLAGS := -m32                     # Compile to 32-bit program
CFLAGS += -fno-builtin             # Do not use GCC built-in functions
CFLAGS += -nostdinc                # Do not include standard headers
CFLAGS += -fno-pic                 # Do not generate position-independent code
CFLAGS += -fno-pie                 # Do not generate position-independent executable
CFLAGS += -nostdlib                # Do not link standard library
CFLAGS += -fno-stack-protector     # Disable stack protection
CFLAGS += -g                       # Add debug info
CFLAGS += -I$(SRC_DIR)/include     # Add include path

# Assembler options (for kernel)
ASMFLAGS := -f elf32 -g -F dwarf

# Linker options
LDFLAGS := -m elf_i386 -static

# ====================================================================
#           Auto-discover sources and generate targets
# ====================================================================
# 1. Find all C source files and kernel assembly files
#    We assume all kernel-related assembly files are in src/kernel
C_SOURCES   := $(shell find $(SRC_DIR) -name '*.c' -not -path '$(SRC_DIR)/test/*')
ASM_SOURCES := $(shell find $(SRC_DIR)/kernel -name '*.asm' -not -path '$(SRC_DIR)/test/*')

# 2. Generate corresponding .o target file list in build directory
C_OBJS   := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJS := $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# 3. Specify the kernel entry object, and exclude it from the total .o list 
#    to ensure it's the first in linking
ENTRY_OBJ  := $(BUILD_DIR)/kernel/start.o
OTHER_OBJS := $(filter-out $(ENTRY_OBJ), $(C_OBJS) $(ASM_OBJS))

# ====================================================================
#                            Build Rules
# ====================================================================
# Default target
all: $(BUILD_DIR)/master.img

# --- Bootloader Rules ---
# Compile boot.asm and loader.asm
$(BUILD_DIR)/boot/%.bin: $(SRC_DIR)/boot/%.asm
	@mkdir -p $(dir $@)
	$(ASM) -f bin $< -o $@

# --- Generic Compilation Rules ---
# This generic rule handles all .c files under any src subdirectory
# e.g.: src/main.c -> build/main.o
#       src/fs/fat32.c -> build/fs/fat32.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "CC $<"
	$(CC) $(CFLAGS) -c $< -o $@

# This generic rule handles all kernel .asm files under any src subdirectory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	@echo "ASM $<"
	$(ASM) $(ASMFLAGS) $< -o $@

# --- Link Kernel ---
$(BUILD_DIR)/kernel.bin: $(ENTRY_OBJ) $(OTHER_OBJS)
	@mkdir -p $(dir $@)
	@echo "LD $@"
	$(LD) $(LDFLAGS) $(ENTRY_OBJ) $(OTHER_OBJS) -o $@ -Ttext $(ENTRYPOINT)

# --- Generate Final Image File ---
$(BUILD_DIR)/system.bin: $(BUILD_DIR)/kernel.bin
	objcopy -O binary $< $@

$(BUILD_DIR)/system.map: $(BUILD_DIR)/kernel.bin
	nm $< | sort > $@

$(BUILD_DIR)/master.img: $(BUILD_DIR)/boot/boot.bin \
                        $(BUILD_DIR)/boot/loader.bin \
                        $(BUILD_DIR)/system.bin \
                        $(BUILD_DIR)/system.map
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD_DIR)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD_DIR)/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc

# ====================================================================
#                    Helper Commands (PHONY targets)
# ====================================================================
.PHONY: all clean bochs qemu qemug vmdk test

test: all

clean:
	rm -rf $(BUILD_DIR)

bochs: $(BUILD_DIR)/master.img
	bochs -q -f ./bochs/bochsrc

qemu: $(BUILD_DIR)/master.img
	qemu-system-i386 \
		-m 32M \
		-boot c \
		-hda $<

qemug: $(BUILD_DIR)/master.img
	qemu-system-i386 \
		-s -S \
		-m 32M \
		-boot c \
		-hda $<

$(BUILD_DIR)/master.vmdk: $(BUILD_DIR)/master.img
	qemu-img convert -O vmdk $< $@

vmdk: $(BUILD_DIR)/master.vmdk