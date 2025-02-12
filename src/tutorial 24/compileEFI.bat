gcc -Wall -Werror -m64 -mabi=ms -I ThatCLib -c efimain.c -o efimain.o
gcc -Wall -Werror -m64 -mabi=ms efimain.o -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main -o BOOTX64.EFI

gcc -Wall -Werror -m64 -mabi=ms -ffreestanding -Wl,--oformat=binary -e main -c loader/loader.c -o loader/loader.bin

@echo off
del /Q *.o
@echo on

pause
