@echo off
del Kernel.iso

call build.bat

bash -c "grub-mkrescue -o Kernel.iso bin"

rem Kernel.bxrc
start qemu-system-x86_64 -cpu Nehalem -m 512 -cdrom Kernel.iso -no-shutdown -monitor telnet:127.0.0.1:1234,server,nowait
