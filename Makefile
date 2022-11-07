mm = heap

run:
	sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -k en-us

debug:
	sudo qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -k en-us


all: bootloader kernel userland image

# Realiza el testeo de PVS Studio
pvs_studio: clean all run_pvs_studio show_pvs_studio_results remove_pvs_studio_files

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

open_docker:
	@docker run -v "${PWD}:/root" --privileged --rm -ti agodio/itba-so:1.0

install_pvs_studio:
	@wget -q -O - https://files.pvs-studio.com/etc/pubkey.txt | apt-key add -
	@wget -O /etc/apt/sources.list.d/viva64.list https://files.pvs-studio.com/etc/viva64.list
	@apt-get install apt-transport-https
	@apt-get update
	@apt-get install pvs-studio
	@pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"

run_pvs_studio:
	@pvs-studio-analyzer trace -- make all
	@pvs-studio-analyzer analyze
	@plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

show_pvs_studio_results:
	@echo "Errores reportados por PVS Studio:\n"
	@cat report.tasks

remove_pvs_studio_files:
	@rm -f PVS-Studio.log report.tasks strace_out

clean:
	#cd Toolchain; make clean
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean
