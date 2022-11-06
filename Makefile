mm = heap

run:
	sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -k en-us

debug:
	sudo qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -k en-us


all: bootloader kernel userland image

#toolchain:
	#cd Toolchain; make all

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all mm=$(mm)

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

clean:
	#cd Toolchain; make clean
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean
