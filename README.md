# ecreg-dump
Utility to simplify dumping EC registers

Build with: 
`gcc -o ecreg-dump ecreg-dump.c`

You will need the ec_sys module loaded with write_support enabled.
On Fedora - so the ec_sys module is available need to run the debug kernel. Do:
`sudo dnf install kernel-debug`

Then load ec_sys module, with write support so we can access extra hidden registers
`sudo modprobe ec_sys write_support=1`

Recommend unloading when done (`rmmod ec_sys`)
