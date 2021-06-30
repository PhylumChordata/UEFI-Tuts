gcc -Wall -Werror -m64 -mabi=ms -ffreestanding -I ThatCLib -c efimain.c -o efimain.o
gcc -Wall -Werror -m64 -mabi=ms efimain.o -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main -o BOOTX64.EFI

del *.o

gcc -Wall -Werror -m64 -mabi=ms -Wl,--oformat=binary -e main -c loader/loader.c -o loader.bin

pause
