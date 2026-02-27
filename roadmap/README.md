## 🧭 Project Roadmap

### ✅ 1. Basic Bootloader Development
The bootloader is the very first program executed by the system after BIOS.  
Goals for this stage:

- [x] Set up 16-bit real mode boot sector
- [x] Implement disk reading (BIOS interrupts / CHS & LBA basics)
- [x] Display text on the screen (teletype mode)
- [x] Load second-stage bootloader
- [x] Switch from real mode → protected mode
- [x] Set up GDT (Global Descriptor Table)
- [x] Jump to kernel entry point


**Technologies & Concepts:**
- x86 Real Mode
- BIOS interrupts (INT 0x10, INT 0x13)
- Assembly (NASM)
- Memory segmentation
- Disk sector loading
- Protected mode transition

---


### Kernel Development
Once in protected mode, the kernel takes over.

Goals for this stage:

#### 🟦 Early Kernel Setup
- [x] Kernel linker script
- [x] Basic C environment setup
- [x] VGA text mode driver
- [x] Global constructors support

#### 🟩 Memory Management
- [x] Paging (setup and enable)
- [x] Physical memory manager
- [x] Basic virtual memory manager

#### 🟨 Hardware & Devices
- [x] Interrupt Descriptor Table (IDT)
- [x] PIC (8259) remapping
- [x] Exception & IRQ handlers
- [x] Keyboard driver
- [x] PIT (Programmable Interval Timer)
- [x] CMOS RTC

#### 🟥 Future Modules (Optional)
- [ ] Simple filesystem
- [x] Syscalls
- [ ] Basic multitasking
- [ ] User mode
- [ ] Shell

---


### ✅ 2. Implement Interrupts & Exception Handling (IDT)
- [x] Research: Read OSDev.org wiki pages for "IDT" and "ISRs".
- [x] Create Structures: Define the C structures for an IDT entry (IdtEntry) and the IDT pointer (IdtPtr).
- [x] Write ISR Stubs: Write basic assembly language stubs for the first 32 CPU exceptions (e.g., Divide by Zero, General Protection Fault).
- [x] Create C Handler: Write a generic C-based interrupt handler function that the assembly stubs can call (it should print an error message).
- [x] Load the IDT: Write an assembly function (lidt) to load the IDT pointer.
- [x] Test Exceptions: Temporarily add int x = 1 / 0; to your kernel and verify that your "Divide by Zero" error message appears (instead of a reboot).
- [x] Enable Interrupts: Add the sti instruction after the IDT is loaded to enable hardware interrupts.

### ✅ 3. Implement Keyboard Input (IRQs & PIC)
- [x] Research: Read OSDev.org wiki pages for "PIC" (Programmable Interrupt Controller) and "PS/2 Keyboard".
- [x] Remap the PIC: Write the code to remap the PIC (IRQ 0-7 -> 32-39, IRQ 8-15 -> 40-47). This is critical to avoid conflicts with CPU exceptions.
- [x] Write Keyboard ISR: Add an ISR for IRQ 1 (interrupt 33 after remapping).
- [x] Handle EOI: In the ISR, send the "End of Interrupt" (EOI) signal to the PIC.
- [x] Read Scancode: In the ISR, read the scancode from the keyboard data port (0x60).
- [x] Create Scancode Map: Create a simple array to translate scancodes to ASCII characters (e.g., US QWERTY).
- [x] Test: Create a basic "echo" function. On each keypress, translate the scancode and print the character to the screen.

### ✅ 4. Implement Paging (Virtual Memory)
- [x] Research: Read OSDev.org wiki page for "Paging" and "Physical Memory Management".
- [x] Get Memory Map: (In boot.asm) Use int 0x15, eax = 0xE820 to get a memory map from the BIOS. Save this map somewhere safe (e.g., at 0x1000).
- [x] Pass Memory Map: Pass the location of the memory map to your C kernel.
- [x] Create PMM: Write a simple Physical Memory Manager (PMM), often called a "bitmap allocator," to keep track of free/used 4KB physical pages.
- [x] Create Page Structures: Define C structures for a Page Directory and Page Tables.
- [x] Map Kernel: Write functions to map virtual addresses to physical addresses. Map your kernel (at 0x8000) and the VGA buffer (0xB8000).
- [x] Enable Paging: Write the assembly code to load the Page Directory address into the CR3 register and set the paging bit in CR0.
- [x] Test: Verify your kernel "Hello World" message still prints after paging is enabled.

### ✅ 5. Implement System Timer (PIT)
- [x] Research: Read OSDev.org on the "Programmable Interval Timer" (PIT).
- [x] Write Timer ISR: Add an ISR for IRQ 0 (interrupt 32 after remapping).
- [x] Set Timer Frequency: Write code to set the PIT frequency (e.g., 100 Hz).
- [x] Create Tick Counter: In the ISR, increment a global kernel_ticks variable and send an EOI to the PIC.
- [x] Test: Write a simple function to print the kernel_ticks to the screen and watch the counter go up. This is the "heartbeat" of your OS.

### ✅ 6. Enter User-Mode & Implement System Calls
- [ ] Research: Read OSDev.org on "Getting to User Mode", "TSS", and "System Calls".
- [ ] Update GDT: Add Ring 3 (user) code and data segment descriptors to your GDT.
- [ ] Create TSS: Create a Task State Segment (TSS) and a GDT entry for it. The TSS tells the CPU where to find the kernel's stack when an interrupt happens in user-mode.
- [ ] Write Syscall Handler: Set up an interrupt handler for a chosen vector (e.g., int 0x80). This will be your system call gate.
- [ ] Implement puts Syscall: Make your syscall handler check a number (e.g., in EAX) and, if it's 1, call your kernel's puts function.
- [ ] Create Test Program: Write a tiny user-mode test function (it can be an array of bytes in your kernel for now) that loads a string address and calls int 0x80.
- [ ] Jump to User-Mode: Set up a user-mode stack and use the iret instruction to jump from Ring 0 (kernel-mode) "down" to Ring 3 (user-mode) and execute your test program.
- [ ] Test: Verify your user program runs and its int 0x80 call successfully prints a message to the screen using your kernel's puts function.



---

## 📦 Build & Run

### **Requirements**
- `nasm`
- `gcc` cross-compiler
- `qemu` or `bochs`

### **Build**
```sh
make all

### Run & debugging

make run
make debug
gdb 
(gdb) symbol-file stage2.elf 
(gdb) target remote localhost:1234
(gdb) break _start
