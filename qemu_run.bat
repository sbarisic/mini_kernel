@echo off
del Kernel.iso

call build.bat

bash -c "grub-mkrescue -o Kernel.iso bin"

rem Kernel.bxrc
start qemu-system-x86_64 -cpu Nehalem -m 666 -cdrom Kernel.iso -no-shutdown -monitor telnet:127.0.0.1:1234,server,nowait -chardev file,id=f0,path=E:\Projects2018\mini_kernel\_com1.txt -chardev file,id=f1,path=E:\Projects2018\mini_kernel\_com2.txt -device pci-serial-2x,chardev1=f0,chardev2=f1
start telnet 127.0.0.1 1234
exit