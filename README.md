# Toy kernel – A Hobby System

This is a toy kernel built from scratch for learning low-level development, bootloaders, CPU architecture, memory management, and kernel internals.  
This project follows a structured roadmap starting from a simple bootloader to building a functional kernel.


# 🧭 Kernel Development Roadmap

## 🧠 Core Principle

Build **capabilities**, not features.

Goal:

> A system that can run programs, isolate them, access files, and communicate safely.

---

## 🔹 PHASE 0 — Boot & Kernel Base

* Bootloader → kernel load
* Basic screen output
* Interrupt setup (IDT)
* Timer interrupt
* Basic keyboard input

**Test:** Interrupts must be stable.

---

## 🔹 PHASE 1 — Execution Model

* Context switching
* Scheduler (round-robin)
* Kernel stack per task
* User mode switch (ring 3)

**Test:**

* Run 2 infinite loops → both execute

---

## 🔹 PHASE 2 — System Call Interface

* Syscall entry (int 0x80 / syscall)
* Syscall dispatch table
* Basic syscalls:

  * write
  * read
  * exit

**Test:**

* User program prints via syscall

---

## 🔹 PHASE 3 — Memory Management (CRITICAL)

* Enable paging
* Per-process address space
* Kernel mapped globally
* User vs kernel permissions
* CR3 switch during context switch

**Test:**

* Process A memory ≠ Process B memory
* User cannot access kernel space

---

## 🔹 PHASE 4 — Process Model

* Process structure
* fork()
* exec()
* wait()
* Process states:

  * running
  * ready
  * blocked
  * zombie

**Test:**

* Shell spawns program → program exits → shell continues

---

## 🔹 PHASE 5 — ELF Loader

* Parse ELF
* Map segments
* Set entry point
* Switch to user mode

**Test:**

* Run compiled user programs

---

## 🔹 PHASE 6 — Virtual File System (VFS)

* File abstraction:

  * open
  * read
  * write
  * close
* File descriptors
* VFS layer (independent of FS)

---

## 🔹 PHASE 7 — Device Drivers

* Keyboard driver (interrupt-based)
* Timer
* Disk driver (ATA / AHCI)

---

## 🔹 PHASE 8 — Real Filesystem

Choose one:

* FAT32 (easy)
* EXT2 (better)

Implement:

* File reading
* Directory traversal
* Metadata

**Test:**

* ls
* cat file

---

## 🔹 PHASE 9 — Userland Expansion

### Shell

* Input handling
* Command parsing
* Program execution

### Utilities

* ls
* cat
* echo

---

## 🔹 PHASE 10 — Memory Management (Level 2)

* User heap (malloc)
* Kernel heap improvements
* Page allocation system

**Test:**

* Dynamic memory works in user programs

---

## 🔹 PHASE 11 — IPC (Inter-Process Communication)

* Pipes
* Basic message passing

**Test:**

```
cat file | grep something
```

---

## 🔹 PHASE 12 — Signals & Process Control

* Kill processes
* Signal handling
* Async events

---

## 🧪 FINAL SYSTEM TEST

Your Kernel should support:

```
shell
 ├── runs ls
 ├── runs cat
 ├── spawns processes
 ├── pipes output
 └── system does NOT crash
```

---