clang \
  -m32 \
  -target i386-pc-none-elf \
  -ffreestanding \
  -nostdlib \
  -fno-pic \
  -fno-pie \
  -fno-stack-protector \
  -fno-asynchronous-unwind-tables \
  -fno-unwind-tables \
  -fno-builtin \
  -mno-sse \
  -mno-mmx \
  -O0 \
  -no-pie \
  -static \
  -Wl,--oformat=binary \
  -Wl,--section-start=.boot=0x7C00 \
  -Wl,--section-start=.kernel=0x7E00 \
  -Wl,--section-start=.rodata=0x7EAC \
  -Wl,-e,boot_entry \
  -Wl,--build-id=none \
  onefile.c -o os.bin

truncate -s 1024 os.bin

clang \
  -m32 \
  -target i386-pc-none-elf \
  -ffreestanding \
  -nostdlib \
  -fno-pic \
  -fno-pie \
  -fno-stack-protector \
  -fno-asynchronous-unwind-tables \
  -fno-unwind-tables \
  -fno-builtin \
  -mno-sse \
  -mno-mmx \
  -O0 \
  -no-pie \
  -static \
  -Wl,--section-start=.boot=0x7C00 \
  -Wl,--section-start=.kernel=0x7E00 \
  -Wl,--section-start=.rodata=0x7EAC \
  -Wl,-e,boot_entry \
  -Wl,--build-id=none \
  onefile.c -o debug.elf

objdump -D -Mintel -mi8086 \
  --start-address=0x7c20 \
  --stop-address=0x7c50 debug.elf

qemu-system-i386 \
  -drive format=raw,file=os.bin \
  -no-reboot \
  -no-shutdown \
  -d cpu \
  -D qemu.log
