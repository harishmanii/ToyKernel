# OS Development Roadmap

---

## ✅ Already Built (Your Actual Baseline)

These are fully working in your kernel right now:

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

# Phase 1 — Preemptive Scheduling

> **Why first:** Your scheduler is cooperative right now. Real OSes never rely on tasks yielding voluntarily. This is the most fundamental OS concept.

### Steps

- [ ] Call `schedule()` from inside the PIT IRQ0 handler (every N ticks)
- [ ] Make sure `switch_task` is safe to call from an interrupt context (save/restore all regs via the IRQ frame)
- [ ] Add a `time_slice` counter per task — only switch after N ticks, not every tick
- [ ] Test: create two tasks that each print their PID in a loop without ever calling `schedule()` — both should run

### What you learn
- How the CPU is "time-shared" between tasks
- Why saving the full register state on the stack is critical
- The difference between cooperative and preemptive multitasking

---

# Phase 2 — Blocking Scheduler + Sleep

> **Why next:** BLOCKED state already exists in your enum but nothing uses it. This makes multitasking actually useful.

### Steps

- [ ] Implement `task_sleep(uint32_t ms)` — set task state to `TASK_BLOCKED`, store a `wake_tick` in the Task struct
- [ ] In the PIT handler, walk the task list and wake any task whose `wake_tick <= kernel_ticks`
- [ ] Implement `task_block()` / `task_unblock()` — generic block/unblock primitives
- [ ] Wire `sys_sleep` syscall (EBX = ms) to call `task_sleep` (your `syscall_sleep` is already stubbed)
- [ ] Test: user task calls `int 0x80` with sleep syscall, kernel task keeps printing — they should interleave

### What you learn
- Sleep queues and wakeup mechanisms
- How blocking I/O will work later (keyboard read, disk wait)
- Why the scheduler needs to be interrupt-safe

---

# Phase 3 — Per-Process Address Space Isolation

> **Why here:** Right now all tasks share the same page directory. Real process isolation requires each process to have its own virtual address space.

### Steps

- [ ] Add `page_directory_t *page_dir` field to the `Task` struct
- [ ] Write `clone_kernel_page_dir()` — allocate a new page directory with kernel mappings copied in (supervisor-only), user space empty
- [ ] In `create_user_task()`, call `clone_kernel_page_dir()` and assign to the task
- [ ] In `switch_task` (asm), reload CR3 when switching to a task with a different page directory
- [ ] Map user code and stack pages into the task's own page directory (not the kernel's)
- [ ] Test: two user tasks writing to the same virtual address should not affect each other

### What you learn
- Why CR3 is the core of process isolation
- How the kernel is shared (mapped in all page dirs) but protected (supervisor bit)
- The concept of virtual address space as a per-process abstraction

---

# Phase 4 — Page Fault Handler (Robust)

> **Why here:** Once you have per-process page directories, invalid memory accesses must be caught properly, not just panic.

### Steps

- [ ] Your IDT already has exception #14 (page fault) — make its handler read `CR2` (fault address) and the error code
- [ ] Decode the error code bits: `present`, `write`, `user`
- [ ] If fault is in user space (user bit set) → terminate the offending task (`task_exit`), print fault info
- [ ] If fault is in kernel space → full kernel panic (print CR2, error code, halt)
- [ ] Optional: if the fault address falls in a valid unmapped region → allocate a page on demand (demand paging)
- [ ] Test: user task dereferences NULL → should be killed cleanly, kernel keeps running

### What you learn
- How the CPU reports memory errors
- Difference between a recoverable user fault and a kernel panic
- The foundation of demand paging / lazy allocation

---

# Phase 5 — ELF Loader

> **Why here:** Right now user tasks are kernel functions cast to function pointers. A real OS loads programs from disk.

### Steps

- [ ] Learn the ELF32 file format: `Elf32_Ehdr`, `Elf32_Phdr` (program headers)
- [ ] Write `elf_load(uint8_t *elf_data)` — parse the ELF header, validate magic bytes (`0x7F 'E' 'L' 'F'`)
- [ ] For each `PT_LOAD` segment: allocate pages at the given virtual address, copy segment data into them
- [ ] Set up user stack at a fixed virtual address (you already have `USER_STACK_VIRT`)
- [ ] Return the entry point (`e_entry`) and jump to it via `iret` in ring 3
- [ ] For now, embed a tiny ELF binary as a `uint8_t[]` array in your kernel to test (no disk read needed yet)
- [ ] Test: the embedded ELF runs and makes a syscall to print something

### What you learn
- How executables are structured
- How the kernel sets up a process's virtual memory from a file
- The link between the linker, ELF segments, and virtual addresses

---

# Phase 6 — Filesystem (FAT12 → Kernel)

> **Why here:** You already parse FAT12 in stage2. Port that into the kernel so you can load real files from disk.

### Steps

- [ ] Write a kernel ATA PIO driver (read sectors via port I/O — ports `0x1F0`–`0x1F7`)
- [ ] Port your stage2 FAT12 reader into the kernel (mostly reuse the existing code)
- [ ] Implement `fat_open(const char *path)` → returns a buffer with file contents
- [ ] Use `elf_load()` from Phase 5 to load an ELF binary read from the FAT12 disk image
- [ ] Test: place a small ELF binary on the disk image, boot the kernel, have it load and run that binary

### What you learn
- How disk I/O works at the hardware level (ATA PIO)
- How a filesystem maps filenames to raw disk sectors
- The full pipeline: disk → FAT → ELF → running process

---

# Phase 7 — Syscall Expansion + Userland libc

> **Why here:** Now that you can run real programs, they need a proper syscall interface to interact with the kernel.

### Steps

- [ ] Formalize your syscall table (EAX = number, EBX/ECX/EDX = args)
- [ ] Implement `sys_write(int fd, const char *buf, size_t len)` — fd 1 = stdout (VGA)
- [ ] Implement `sys_exit(int code)` — terminates current task cleanly, frees memory
- [ ] Implement `sys_getpid()` — returns current task's PID
- [ ] Validate all user pointers before dereferencing in the kernel (`copy_from_user`)
- [ ] Write a minimal `libc` for user programs: `write()`, `exit()`, `puts()` wrappers around `int 0x80`
- [ ] Test: a user program compiled with your libc that prints "Hello from user space!" and exits cleanly

### What you learn
- The contract between user space and kernel space
- Why user pointer validation is critical (security)
- How real libc wraps syscalls (same pattern as Linux)

---

# Phase 8 — Simple Shell

> **Why here:** Brings everything together. A shell is a user-mode program that uses every subsystem you built.

### Steps

- [ ] Write a kernel keyboard input buffer with a ring buffer — `keyboard_getc()` blocks until a key is pressed
- [ ] Wire a `sys_read(fd, buf, len)` syscall — fd 0 = stdin (keyboard)
- [ ] Write a user-mode shell program (compiled as ELF, loaded from disk):
  - Read a line from stdin using `sys_read`
  - Parse the command string
  - Load and execute commands using the ELF loader
- [ ] Basic built-in commands: `help`, `clear`, `reboot`
- [ ] Test: type a command, kernel loads the ELF binary for it, it runs, exits, shell returns to prompt

### What you learn
- How a shell is just a regular user-mode process
- How `exec`-like behavior works (load new ELF, replace address space)
- End-to-end OS flow: keyboard → syscall → kernel → disk → ELF → process → output

---

# Key Rules

- Do NOT skip phases — each one is a dependency for the next
- Test every phase by intentionally breaking it (null pointer, bad syscall number, etc.)
- Build the minimal version first, then refine
- Read the OSDev wiki page for each topic before implementing it

---

# Immediate Next Step

**Phase 1 — Preemptive Scheduling**

1. Open `src/kernel/interrupts/` and find the PIT IRQ0 handler
2. Add a call to `schedule()` at the end of it
3. Create two tasks that print their PID in an infinite loop without yielding
4. Verify both tasks run
