# Toy kernel – A Hobby System

**Project:**
- **Description:** A small educational OS kernel and bootloader implementation (i686). The repository includes a two-stage bootloader (stage1/stage2), a kernel implemented in C/assembly, a simple userland, and build scripts to assemble a floppy image.
- **Layout:** Key files and helpers are managed by the top-level [Makefile](Makefile). Toolchain build logic lives in [build_scripts/toolchain.mk](build_scripts/toolchain.mk) and configuration defaults are in [build_scripts/config.mk](build_scripts/config.mk).

**Cross-Compiler — What & Why:**
- **What:** A cross-compiler is a compiler that runs on one host architecture (your macOS machine) but generates binaries for a different target architecture (here: `i686-elf`).
- **Why needed:** The kernel and bootloader target a 32-bit i386 environment (bare metal, no OS). The host compiler on macOS builds binaries for macOS; those binaries cannot be linked into an OS image for i686. A cross-compiler with a matching `TARGET` (default `i686-elf`) produces object files, libraries and tools (like `i686-elf-gcc`, `i686-elf-ld`, and `i686-elf-objcopy`) suitable for building a freestanding kernel and userland that will run under QEMU/Bochs or on real hardware.

**Toolchain setup (using the Makefile):**
- **Defaults:** The repository default target is `i686-elf` (see [build_scripts/config.mk](build_scripts/config.mk)). The toolchain is installed under `opt/cross/$(TARGET)` by the included scripts.
- **How it works:** The included `toolchain` target in [build_scripts/toolchain.mk](build_scripts/toolchain.mk) downloads, configures, builds and installs binutils and a minimal GCC (C only) into `opt/cross/i686-elf` so that `opt/cross/i686-elf/bin` contains `i686-elf-gcc`, `i686-elf-ld`, etc.
- **Install dependencies (macOS example):** You will need common build tools. Example (Homebrew):

```bash
brew install wget nasm mtools qemu bochs gmp mpfr libmpc
```

- **Build the cross toolchain:** From the repository root run:

```bash
make toolchain
```

Notes:
- If you prefer to override the default target, export `TARGET` on the make command line, e.g. `make toolchain TARGET=x86_64-elf` and update other build variables accordingly.
- The toolchain build downloads GNU sources and compiles them locally; it can take a long time depending on your machine.

**Build & Run the kernel (using the Makefile):**
- **Top-level targets:** The primary top-level targets are `all` (build image), `clean`, `toolchain`, `run`, and `debug`. See [Makefile](Makefile) for full list.
- **Quick build:**

```bash
# build everything (bootloader, kernel, userland) and create a floppy image
make

# or explicit target
make all
```

- **Run in QEMU:**

```bash
# run the freshly built floppy image in QEMU
make run

# run with the debugger-friendly flags
make debug
```

- **Clean build artifacts:**

```bash
make clean
```

**How the image is constructed:**
- The top-level `floppy_image` target creates `build/image/main_floppy.img`, writes stage1 into the MBR, copies `stage2.bin`, `kernel.bin` and `userland.elf` into the image using `mcopy`/mtools. The relevant rules are implemented in the top-level [Makefile](Makefile).

**Troubleshooting & tips:**
- Ensure `opt/cross/$(TARGET)/bin` is in your `PATH` (the Makefiles already prepend it when building if the toolchain is present). If you installed the toolchain manually or to a custom location, export `PATH` accordingly.
- If `make toolchain` fails due to missing libraries (GMP/MPFR/MPC), install those via Homebrew (see the dependencies command above) or point the configure options in [build_scripts/toolchain.mk](build_scripts/toolchain.mk) to your library prefixes.
- If your host gcc is not `gcc-15` as configured, either install the specified compiler or adjust `CC`, `CXX` and their paths in [build_scripts/config.mk](build_scripts/config.mk).

**Further reading / next steps:**
- Read the blueprints in `blueprints/` for design notes and the `roadmap/` for completed milestones and future goals.
- To extend the kernel, explore `src/kernel`, `src/bootloader` and `src/userland`.

---

If you want, I can also:
- add a brief Quickstart script that runs prerequisites and `make toolchain && make && make run`.
- or add a small troubleshooting section for macOS-specific quirks.

Enjoy building and exploring the kernel!

