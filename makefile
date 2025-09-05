

boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin


master.img: boot.bin
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat master.img
	dd if=boot.bin of=master.img bs=512 count=1 conv=notrunc




.PHONY: clean
clean:
	rm -rf $(BUILD)


.PHONY: bochs
bochs: master.img
	bochs -q -f bochsrc



.PHONY: qemu
qemu: master.img
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