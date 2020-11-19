grub_cfg=util/grub.cfg
kernel=build/kibby.bin

iso=kibbyos.iso

link_script=link.ld

csrc=$(shell find src/ -type f -name '*.c')
cobj=$(patsubst src/%.c,build/%.c.o,$(csrc))
chdr=$(shell find include/ -type f -name '*.h')
chdr += $(shell find src/ -type f -name '*.h')
asrc=$(shell find src/ -type f -name '*.asm')
aobj=$(patsubst src/%.asm,build/%.a.o,$(asrc))
flags=-Iinclude/ -std=gnu11 -ffreestanding -O2 -Wall -Wextra -nostdlib -Werror -Wno-unused-parameter -Wno-unused-variable

paths=std/ kernel/internal
buildpaths=$(patsubst %,build/%,$(paths))

.PHONY: all run bear clean

all: $(iso)

run: all
	qemu-system-i386 \
		-drive format=raw,media=cdrom,file=$(iso) \
		-serial stdio

run-debug: all
	qemu-system-i386 \
		-drive format=raw,media=cdrom,file=$(iso) \
		-serial file:com1.log \
		-s -S \
		-monitor stdio &
	sleep 0.5s
	termite -e 'gdb -tui -q --eval-command="target remote :1234" --eval-command="layout asm" --eval-command="layout reg"'

clean:
	rm -rf iso/*
	rm -rf build/*
	rm $(iso)

bear: clean
	bear -- make

# gathering of iso deps
$(iso): $(bootloader) $(kernel) $(grub_cfg)
	mkdir -p iso/boot/grub
	cp $(grub_cfg) iso/boot/grub/
	cp $(kernel) iso/
	grub-mkrescue -o $(iso) iso/

$(kernel): $(link_script) $(cobj) $(aobj)
	# first dep is the link script so this hacks together nicely
	ld -m elf_i386 -T $^ -o $@

$(cobj): build/%.c.o: src/%.c $(chdr) | $(buildpaths)
	clang -target i686-elf -c $< -o $@ $(flags)

$(aobj): build/%.a.o: src/%.asm
	nasm -f elf32 $^ -o $@

$(buildpaths):
	mkdir -p $@
