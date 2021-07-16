gcc -Wall -Werror -m64 -mabi=ms -I ThatCLib -c efimain.c -o efimain.o
gcc -Wall -Werror -m64 -mabi=ms efimain.o -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main -o BOOTX64.EFI

@echo off
del /Q *.o
@echo on

pause
