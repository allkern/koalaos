<p align="center" width="100%">
    <img width="44%" src="https://github.com/allkern/koalaos/assets/15825466/9600800c-01ba-4c0a-b53a-28256aa69362">
</p>

# KoalaOS
A simple, text-based, single-tasking operating system written in C

<p align="center" width="100%">
    <img width="88%" src="https://github.com/allkern/koalaos/assets/15825466/fb1f2cb0-221b-497a-9dae-76a502d6c7ac">
</p>

# Building
You have to get your hands on a MIPS R3000-compatible GCC toolchain, I used `mipsel-linux-gnu-` and this is what the Makefile is currently configured to use.

Once you have an R3000 toolchain, building should be very easy, execute the following commands in this order:
```
make
make kernel
```
Additionally, running `make fs` (needs root access) will copy the compiled binaries to the `root` folder (not `/`), and will generate a disk image for use on an [emulator](https://github.com/allkern/koalaboard).

## Configuration
The `src/config.h` file contains a couple configuration switches:
- `GPU_FONT16/GPU_FONT8` allow selecting the height of the font for the GPU-based renderer
- `TERM_GPU/TERM_UART` allow selecting whether to use the GPU-based terminal or write all output to the emulated UART chip

# Using
> [!WARNING]
> KoalaOS has become somewhat unstable after the introduction of dynamic memory mapping, use at your own risk

There's no real reason to use this OS other than fun or torture, but if you decide to do so anyways, here's some useful information!

(WIP)
