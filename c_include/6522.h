// Define the 6522 structure
// Using __attribute__((packed)) can prevent compiler padding issues if needed.
// However, with 1-byte registers and standard 1-byte alignment, it is usually okay.
struct mos6522_regs {
    volatile char orb_irb;     // 0x00: Output Register B / Input Register B
    volatile char ora_ira;     // 0x01: Output Register A / Input Register A
    volatile char ddrb;        // 0x02: Data Direction Register B
    volatile char ddra;        // 0x03: Data Direction Register A
    volatile char t1cl;        // 0x04: Timer 1 Counter Low
    volatile char t1ch;        // 0x05: Timer 1 Counter High
    volatile char t1ll;        // 0x06: Timer 1 Latch Low
    volatile char t1lh;        // 0x07: Timer 1 Latch High
    volatile char t2cl;        // 0x08: Timer 2 Counter Low
    volatile char t2ch;        // 0x09: Timer 2 Counter High / Latch High
    volatile char sr;          // 0x0A: Shift Register
    volatile char acr;         // 0x0B: Auxiliary Control Register
    volatile char pcr;         // 0x0C: Peripheral Control Register
    volatile char ifr;         // 0x0D: Interrupt Flag Register
    volatile char ier;         // 0x0E: Interrupt Enable Register
    volatile char ora_nohs;    // 0x0F: Output Register A (no handshake)
};

