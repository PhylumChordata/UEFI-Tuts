OSFMount -a -t file -f ..\..\drive\drive.hdd -s 40M -o rw -m F:

copy /y BOOTX64.EFI "F:\EFI\Boot\"

@echo off
rem move /y gdt\gdt.bin "F:\ThatOS64\"
@echo on

move /y loader\loader.bin "F:\ThatOS64\"

timeout 3

OSFMount -D -m F:

pause

