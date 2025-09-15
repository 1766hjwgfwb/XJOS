BUILD := build
SRC := src
BOCHS_DIR := bochs
ENTRYPOINT := 0x10000

# default target
all: $(BUILD)/master.img

#----------------------------------------
# Boot sector (bin)
#----------------------------------------
$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm | $(BUILD)/boot
	nasm -f bin $< -o $@

#----------------------------------------
# Kernel object (elf32)
#----------------------------------------
$(BUILD)/kernel/%.o: $(SRC)/kernel/%.asm | $(BUILD)/kernel
	nasm -f elf32 $< -o $@

#----------------------------------------
# Kernel binary (linked)
#----------------------------------------
$(BUILD)/kernel/kernel.bin: $(BUILD)/kernel/start.o | $(BUILD)/kernel
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPOINT)

#----------------------------------------
# System binary + map
#----------------------------------------
$(BUILD)/system/system.bin: $(BUILD)/kernel/kernel.bin | $(BUILD)/system
	objcopy -O binary $< $@

$(BUILD)/system/system.map: $(BUILD)/kernel/kernel.bin | $(BUILD)/system
	nm $< | sort > $@

#----------------------------------------
# Master disk image
#----------------------------------------
$(BUILD)/master.img: \
	$(BUILD)/boot/boot.bin \
	$(BUILD)/boot/loader.bin \
	$(BUILD)/system/system.bin \
	$(BUILD)/system/system.map

	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=$(BUILD)/system/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc

#----------------------------------------
# Run targets
#----------------------------------------
test: $(BUILD)/master.img


.PHONY: bochs qemu qemug clean


bochs: $(BUILD)/master.img
	bochs -q -f $(BOCHS_DIR)/bochsrc

qemu: $(BUILD)/master.img
	qemu-system-i386 -m 32M -boot c -hda $<



qemug: $(BUILD)/master.img
	qemu-system-i386 -s -S -m 32M -boot c -hda $<


#----------------------------------------
# Utils
#----------------------------------------
clean:
	rm -rf $(BUILD)


# Auto create directories if missing
$(BUILD) $(BUILD)/boot $(BUILD)/kernel $(BUILD)/system:
	mkdir -p $@

