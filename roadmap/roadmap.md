# MyKernel — Development Roadmap

> A bare-metal x86 OS written in C + Assembly.  
> This document tracks what is built, what is in progress, and what comes next.

---

## Project Status

| Phase | Title | Status |
|---|---|---|
| Baseline | Core kernel infrastructure | ✅ Complete |
| Phase 0 | Userland Code Organization | ✅ Complete |
| Phase 1 | Preemptive Scheduling | 🔴 In Progress |
| Phase 2 | Blocking Scheduler + Sleep | ⏳ Pending |
| Phase 3 | Per-Process Address Space Isolation | ⏳ Pending |
| Phase 4 | Page Fault Handler | ⏳ Pending |
| Phase 5 | ELF Loader | ⏳ Pending |
| Phase 6 | Filesystem (FAT12 → Kernel) | ⏳ Pending |
| Phase 7 | Syscall Expansion + Userland libc | ⏳ Pending |
| Phase 8 | Simple Shell | ⏳ Pending |

---

## Baseline — Core Kernel Infrastructure ✅ Complete

| Component | Notes |
|---|---|
| Stage1 + Stage2 bootloader | Real mode → Protected mode, FAT12 disk read |
| GDT | Kernel + User segments |
| IDT | 32 CPU exceptions + IRQ handlers |
| PIC (8259) | Remapped, EOI working |
| VGA text driver | Kernel-side `printf` / `put_c` |
| PMM | Bitmap allocator over BIOS memory map |
| VMM + Paging | CR3 loaded, kernel identity-mapped |
| PIT timer | IRQ0 handler, `kernel_ticks` counter |
| Keyboard driver | IRQ1, scancode → ASCII |
| Syscall gate | `int 0x80`, EAX = syscall number, EBX = arg |
| TSS | Set up for ring-3 → ring-0 stack switch |
| Cooperative scheduler | `switch_task` in asm, `create_task`, `schedule()` |
| Ring-3 user tasks | `create_user_task`, `iret` into user mode |
| Task states | `READY`, `RUNNING`, `PAUSED`, `BLOCKED`, `COMPLETED`, `TERMINATED` |

---

## Phase 0 — Userland Code Organization ✅ Complete

**Goal:** Physically separate ring-3 code from kernel code at the linker level.

| Task | Status |
|---|---|
| Created `src/kernel/userland/` directory | ✅ |
| Moved `user_program()` into `user_programs.c` with `.user_text` section tag | ✅ |
| Created `syscall_stubs.c/.h` — `sys_write`, `sys_exit`, `sys_getpid` via `int $0x80` | ✅ |
| Added `.user_text` section to `kernel.ld` at `0x00400000` with `__user_start`/`__user_end` | ✅ |
| Updated `create_user_task()` to map user pages and compute correct `user_eip` | ✅ |
| Removed `user_program()` from `hal.c` | ✅ |
| Verified Makefile picks up `userland/*.c` | ✅ |
| Tested: user task runs from `.user_text` | ✅ |
| Tested: `printf` in ring-3 triggers GP fault (isolation confirmed) | ✅ |

**Rules going forward:**
- Every function running in ring 3 **must** use `__attribute__((section(".user_text")))`
- Ring-3 code may only call the kernel via `int $0x80` — never via function pointers

---

## Phase 1 — Preemptive Scheduling 🔴 In Progress

**Goal:** The kernel forcibly switches tasks on a timer tick. Tasks no longer need to yield voluntarily.

| Task | Status |
|---|---|
| Add `time_slice` field to `Task` struct in `task.h`, initialize to N ticks in `create_task()` | ⬜ |
| In PIT IRQ0 handler: decrement `time_slice`, call `schedule()` when it hits 0, then reset | ⬜ |
| Verify `switch_task` correctly saves/restores all registers when called from interrupt context | ⬜ |
| Test: two tasks printing their PID in a tight loop with no `schedule()` calls — both must run | ⬜ |

**Files to touch:** `src/kernel/interrupts/pit.c`, `src/kernel/task/task.h`, `src/kernel/task/task.c`, `src/kernel/task/switch_task.asm`

---

## Phase 2 — Blocking Scheduler + Sleep ⏳ Pending

**Goal:** Tasks can block and wake up — the foundation for all I/O waiting.

| Task | Status |
|---|---|
| Add `wake_tick` field to `Task` struct | ⬜ |
| Implement `task_sleep(uint32_t ms)` — sets state to `TASK_BLOCKED`, stores `wake_tick` | ⬜ |
| In PIT handler: walk task list, wake any task where `wake_tick <= kernel_ticks` | ⬜ |
| Implement `task_block()` / `task_unblock()` as generic primitives | ⬜ |
| Wire `sys_sleep` syscall (EBX = ms) → `task_sleep()` | ⬜ |
| Test: sleeping user task interleaves with a running kernel task | ⬜ |

---

## Phase 3 — Per-Process Address Space Isolation ⏳ Pending

**Goal:** Each process gets its own page directory — true memory isolation.

| Task | Status |
|---|---|
| Add `page_directory_t *page_dir` to `Task` struct | ⬜ |
| Write `clone_kernel_page_dir()` — new page dir with kernel mappings copied, user space empty | ⬜ |
| Call `clone_kernel_page_dir()` inside `create_user_task()` | ⬜ |
| In `switch_task` asm: reload CR3 if the new task has a different page directory | ⬜ |
| Map user code + stack pages into the task's own page directory | ⬜ |
| Test: two user tasks writing to the same virtual address do not affect each other | ⬜ |

---

## Phase 4 — Page Fault Handler ⏳ Pending

**Goal:** Invalid memory accesses in user space kill the task cleanly. Kernel faults still panic.

| Task | Status |
|---|---|
| Page fault handler (IDT #14) reads `CR2` and the error code | ⬜ |
| Decode error bits: `present`, `write`, `user` | ⬜ |
| User-space fault → terminate offending task, print fault info, continue running | ⬜ |
| Kernel-space fault → full panic (print CR2 + error code, halt) | ⬜ |
| Optional: demand paging — allocate page on first access to valid region | ⬜ |
| Test: user task dereferences NULL → killed cleanly, kernel keeps running | ⬜ |

---

## Phase 5 — ELF Loader ⏳ Pending

**Goal:** Load and run actual ELF binaries instead of kernel function pointers.

| Task | Status |
|---|---|
| Understand ELF32 format: `Elf32_Ehdr`, `Elf32_Phdr` | ⬜ |
| Write `elf_load(uint8_t *data)` — validate magic, parse program headers | ⬜ |
| For each `PT_LOAD` segment: allocate pages, copy data to target virtual address | ⬜ |
| Set up user stack at `USER_STACK_VIRT` | ⬜ |
| Jump to `e_entry` via `iret` in ring 3 | ⬜ |
| Test: embed a tiny ELF as a `uint8_t[]` array, load it, have it make a syscall | ⬜ |

---

## Phase 6 — Filesystem (FAT12 → Kernel) ⏳ Pending

**Goal:** Read real files from disk. Load ELF binaries from the FAT12 image.

| Task | Status |
|---|---|
| Write kernel ATA PIO driver (ports `0x1F0`–`0x1F7`) | ⬜ |
| Port stage2 FAT12 reader into the kernel | ⬜ |
| Implement `fat_open(const char *path)` → file buffer | ⬜ |
| Use `elf_load()` on the file buffer to run it | ⬜ |
| Test: place a small ELF on the disk image, boot, load and run it | ⬜ |

---

## Phase 7 — Syscall Expansion + Userland libc ⏳ Pending

**Goal:** Formalize the syscall ABI and give user programs a proper C library.

| Task | Status |
|---|---|
| Formalize syscall table (EAX = number, EBX/ECX/EDX = args) | ⬜ |
| `sys_write(fd, buf, len)` — fd 1 = VGA stdout | ⬜ |
| `sys_exit(code)` — clean task teardown + memory free | ⬜ |
| `sys_getpid()` — returns current task PID | ⬜ |
| `copy_from_user()` — validate all user pointers before kernel dereference | ⬜ |
| Minimal userland libc: `write()`, `exit()`, `puts()` wrapping `int $0x80` | ⬜ |
| Test: user program prints "Hello from user space!" and exits cleanly | ⬜ |

---

## Phase 8 — Simple Shell ⏳ Pending

**Goal:** A working interactive shell as a user-mode process — everything comes together here.

| Task | Status |
|---|---|
| Keyboard ring buffer in kernel — `keyboard_getc()` blocks until key pressed | ⬜ |
| `sys_read(fd, buf, len)` syscall — fd 0 = keyboard stdin | ⬜ |
| User-mode shell: reads a line, parses command, loads ELF and runs it | ⬜ |
| Built-in commands: `help`, `clear`, `reboot` | ⬜ |
| Test: type a command → kernel loads its ELF → it runs → exits → shell returns to prompt | ⬜ |

---

## Rules

- **Do not skip phases** — each one is a hard dependency for the next
- Always test by intentionally breaking things (null pointer, bad syscall, wrong ELF magic)
- Build the minimal working version first, polish later
- Any code running in ring 3 must be in `.user_text` and may only call the kernel via `int $0x80`
