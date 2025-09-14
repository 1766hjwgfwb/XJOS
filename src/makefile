BUILD = ../build
SRC:=.


$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@
# why need to use '-f', "chatgpt"


$(BUILD)/master.img: $(BUILD)/boot/boot.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc


.PHONY: clean
clean:
	rm -rf $(BUILD)


.PHONY: bochs
bochs: $(BUILD)/master.img
	bochs -q -f bochsrc



.PHONY: qemu
qemu: $(BUILD)/master.img
	qemu-system-i386 \
	-m 32M \
	-boot c \
	-hda $<



.PHONY: qemug
qemug: master.img
	qemu-system-i386 \
	-s -S \
	-m 32M \
	-boot c \
	-hda $<