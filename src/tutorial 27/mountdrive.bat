OSFMount -a -t file -f ..\..\drive\drive.hdd -s 40M -o rw -m F:

copy /y BOOTX64.EFI "F:\EFI\Boot\"

timeout 3

OSFMount -D -m F:

pause

