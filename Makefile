.PHONY: kernel

.ONESHELL:

CROSS_PREFIX := mipsel-linux-gnu

CC := gcc
CFLAGS := -g -DLOG_USE_COLOR -Ofast -lSDL2 -lSDL2main -Wno-unused-result

KERNEL_SOURCES := kernel/kernel.s

KOS_CC := gcc
KOS_CFLAGS := -static -nostdlib -EL -fno-tree-loop-distribute-patterns -O3
KOS_CFLAGS += -march=r3000 -mtune=r3000 -mfp32 -ffreestanding -nostdinc
KOS_CFLAGS += -Wl,-T src/script.ld
KOS_SOURCES := $(wildcard src/*.c)
KOS_SOURCES += $(wildcard src/libc/*.c)
KOS_SOURCES += $(wildcard src/sys/*.c)
KOS_SOURCES += $(wildcard src/usr/*.c)
KOS_SOURCES += $(wildcard src/hw/*.c)
KOS_SOURCES += $(wildcard src/util/*.c)

VERSION_TAG := $(shell git describe --always --tags --abbrev=0)
COMMIT_HASH := $(shell git rev-parse --short HEAD)
OS_INFO := $(shell uname -rmo)

bin/system src/main.c:
	mkdir -p bin

	$(CROSS_PREFIX)-$(KOS_CC) \
		$(KOS_SOURCES) \
		-o bin/system \
		-Isrc \
		-DVERSION_TAG="$(VERSION_TAG)" \
		-DCOMMIT_HASH="$(COMMIT_HASH)" \
		-DOS_INFO="$(OS_INFO)" \
		$(KOS_CFLAGS)

fs:
	cp bin/* root/
	sudo ./mkfs.sh
	sudo chmod a+rw disk.img

kernel:
	mkdir -p build
	mkdir -p bin

	$(CROSS_PREFIX)-as $(KERNEL_SOURCES) -o build/kernel.o -mips1 -EL
	$(CROSS_PREFIX)-ld build/kernel.o -T kernel/script.ld -o bin/kernel

clean:
	rm -rf bin
	rm -rf build
