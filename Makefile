
all: bootloader kernel userland image

#toolchain:
	#cd Toolchain; make all
mm = heap
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
