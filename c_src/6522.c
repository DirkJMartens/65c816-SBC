// Define a pointer to the VIA at a specific base address
// Example: If your VIA is mapped to base address 0xA000
#include "6522.h" 
#define VIA_BASE_ADDRESS 0xA000 

struct mos6522_regs * const VIA = (struct mos6522_regs *)VIA_BASE_ADDRESS;

// Example of how to use it
void setup_via(void) {
    // Set Port B to all outputs (write 0xFF to DDRB)
    VIA->ddrb = 0xFF;
    
    // Set Port A to all inputs (write 0x00 to DDRA)
    VIA->ddra = 0x00;

    // Enable Timer 1 interrupts (set bit 7 of IER to enable, and set the T1 bit)
    // The datasheet explains how to enable specific interrupts
    VIA->ier = 0xC0; 
}

void main_loop(void) {
    // Write a value to Output Register B
    VIA->orb_irb = 0x55;

    // Read a value from Input Register A
    char input_a = VIA->ora_ira;
}
