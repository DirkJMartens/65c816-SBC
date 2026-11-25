The **Western Design Center (WDC) 65816** toolchain `(wdc816cc, wdc816as, and wdcln)` is derived from older 1990s compiler technology (originally based on Lichtman/Orca-era tools) and has several well-known bugs, C standard limitations, and 65816-specific traps that regularly trip up homebrew developers.
Here are the key issues and idiosyncrasies to watch for in your project.

# 1. WDC C Compiler ('wdc816cc') Quirks & Bugs
- **Strict C89 / Pre-C99 Syntax Only:**
	- You _cannot_ declare variables in the middle of a block or inside for ('int i = 0; ...') loops—all local variables must be declared at the very top of a function or '{ ... }' block.
	- Standard modern headers like '<stdint.h>' and '<stdbool.h>' do not exist, nor do '0b...' binary literals (only hex, octal and decimal literals can be used). Stick to 'unsigned char' (8-bit), 'unsigned int' (16-bit), 'unsigned long' (32-bit, though pointers are 24-bit in large model), and '0x' hex constants.
- **Optimizer ('-SO') Bugs on 'volatile' & Bitwise Operations:**
	- The WDC optimizer is notorious for occasionally eliminating or reordering reads/writes to hardware I/O registers—even when marked 'volatile'—or miscompiling complex compound bit-shift expressions.
	- Rule of thumb: Always compile without '-SO' first when debugging hardware drivers (like your ACIA or VIA2 LCD routines). If an optimized build breaks I/O timing, inspect the generated '.asm' output ('-BS flag') to verify the compiler actually emitted the 'LDA/STA' instructions.
- **Large Memory Model ('-ML') Pointer & Bank Traps:**
	- In '-ML' (Large) model, pointers are 24-bit ('far'), and string literals/constants may be placed in a different bank ('DBR' vs 'K/Program Bank') than your RAM data.
	- Passing a pointer to a standard library function or assembly routine that expects a 16-bit ('near') pointer—or vice versa—silently truncates the bank byte and reads/writes from the wrong bank.
- **16-Bit 'int' Overflow on Address Math:**
	- 'int' is 16 bits ('-32768 to 32767'). Casting a 16-bit address like '0x8000' or '0xFE00' without 'unsigned int' or 'unsigned long' ('0x007FE0UL') can trigger sign-extension into the bank byte ('$FF8000' instead of '$008000'). Always suffix hardware addresses with 'UL' when casting to pointers.

# 2. WDC Assembler ('wdc816as') Quirks & Bugs
- **Assembler Width Directives vs. Runtime CPU State:**
	- 'wdc816as' does not automatically track 'SEP' and 'REP' instructions at assembly time. If you execute 'REP #$20' (16-bit accumulator) without explicitly writing 'LONGA ON' right after it, 'LDA #$1234' will assemble as a 2-byte instruction instead of 3 bytes, corrupting the instruction stream and crashing the CPU.
	- Always pair 'REP #$20 / SEP #$20' with 'LONGA ON / LONGA OFF', and 'REP #$10 / SEP #$10' with 'LONGI ON / LONGI OFF'.
- **24-Bit vs. 16-Bit Address Mode Ambiguity:**
	- If a label is defined *after* it is referenced (a forward reference), `wdc816as` may default to 16-bit absolute addressing (`LDA label`) rather than 24-bit long addressing (`LDA >label`), or fail if the label is in another bank. Use explicit size prefixes (`<` for Direct Page, `| or !` for 16-bit absolute, `>` for 24-bit long) whenever bank crossing matters.
- **Case Sensitivity & Directive Quirks:**
	- By default, `wdc816as` is case-sensitive for labels unless `-C` (ignore case) is passed, and C functions exported to assembly usually carry a leading underscore (`_load_record`) or `~~` prefix depending on compiler flags. Check your `.lst` or `.map` file to see exact mangled symbol names.
- **Emulation Mode vs. Native Mode Reset State:**
	- The W65C816S always powers on in 6502 Emulation mode (E=1, 8-bit M/X) at $00:FFFC. Your startup assembly file must explicitly execute `CLC + XCE` and `REP #$30` (plus `LONGA ON / LONGI ON`) before jumping into compiled C code, because `wdc816cc` assumes native 16-bit mode.
- **EQU should not have spaces before or after a '+'**
 	- This means you should not use 'STAT EQU BASE + 1' but use 'STAT EQU BASE+1' instead 
- **Do not use REPT macro**
	- Do not use te "REPT" macro to include repeating opcodes (e.g. "REPT NOP") 

# 3. WDC Linker (wdcln) Quirks & Bugs
- **Strict Command-Line Flag Order & Comma Syntax:**
	- `wdcln` is very picky about flag formatting: addresses take `OFFSET,BANK` without spaces (e.g., `-C8000,0000 -D0200,0000`), and library flags (`-LCL` for Large C library) must appear _after_ your `.obj` files on the command line so unresolved symbols are pulled in properly.
- **Bank Boundary Crossing (`$FFFF -> $0000`):**
	- The W65C816S program counter (`PC`) wraps within the current 64KB bank (`$00:FFFF` wraps to `$00:0000`, not `$01:0000`). `wdcln` will warn or misplace code if a single `CODE` or `DATA` section overflows a 64KB bank boundary unless split across segments.
- **Raw Binary (`-HB`) Padding & Vector Placement:**
	- When outputting a raw `.bin` file (`-HB`) for an EEPROM programmer, `wdcln` only writes bytes between your lowest and highest linked addresses. If your reset/interrupt vectors at `$00:FFE4–$00:FFFF` aren't explicitly placed in a fixed segment at the top of your ROM space, your `.bin` file will be shorter than 32KB (`0x8000` bytes) and your vectors will end up at the wrong EEPROM offset when burned.
    - Always check the generated `.bin` file size (e.g., exactly `32,768` bytes for a 28C256 starting at `$8000`) and inspect the last 32 bytes in a hex editor before burning an EEPROM.
- **Initialized Global Variables (`DATA` vs. `UDATA`):**
	- In a ROM-based homebrew computer, initialized C globals (like `char msg[] = "Hello";`) are linked into a `DATA` segment. If `DATA` is placed in ROM (`$8000+`), writing to that array at runtime will silently fail; if `DATA` is placed in RAM (`$0200+`), your startup assembly (`cstartup.asm`) must copy the initial values from ROM to RAM before calling `main()`.

# Quick Defensive Checklist for Your CLAUDE.md / copilot-instructions.md
Adding these 4 rules to your AI instructions file prevents the AI from generating code that hits these WDC traps:
1. *"Always suffix hardware addresses with UL (e.g., `0x007FE0UL`) to prevent 16-bit signed int overflow."*
2. *"Never use C99 features (// is okay in newer WDC builds, but avoid mid-block variable declarations, `<stdint.h>`, or 0b binary literals)."*
3. *"Always pair `SEP/REP` instructions in assembly with `LONGA/LONGI` directives."*
4. *"Verify that the linked `.bin` output is padded to the exact EEPROM size with vectors at `$FFE4–$FFFF`."*
