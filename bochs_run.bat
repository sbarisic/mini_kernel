@echo off
del Kernel.iso

call build.bat

bash -c "grub-mkrescue -o Kernel.iso bin"

Kernel.bxrc

