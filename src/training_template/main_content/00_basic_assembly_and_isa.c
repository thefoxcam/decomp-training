
/* ================================================================ *
 *
 * Welcome to decomp!
 *
 * This series of example files is designed to teach you the basics of
 * decompilation for the Gamecube and Wii. It is recommended to read this
 * accompanying article on the wiki before getting started:

 * https://wiki.decomp.dev/en/resources/decomp-intro
 *
 * ================================================================ */

/* ================================================================ *

 * **** CHAPTER 0 - BASIC ASSEMBLY AND ISA OPERATIONS

 * The first place you should look when you encounter an instruction
 * you are unfamiliar with is here:

 * https://files.decomp.dev/ppc_isa.pdf

 * This is a specification of what is known as an instruction set
 * architecture (ISA), which defines various fundamental properties of
 * a CPU like what instructions exist, how they behave, and what
 * registers are available [1].

 * The implementations of each function in this section are already
 * filled out, as jumping straight into writing functions will be a
 * bit confusing without learning about how functions are defined
 * (covered in the next section). For now simply uncomment the code
 * lines and observe the assembly; also consider looking up an
 * instruction in the ISA document to get a feel for how the
 * ISA definitions are written.
 *
 * [1] IBM additionally added SIMD-type instructions for
 * floating-point registers on the GC/Wii's processors known as
 * "paired singles" which can be viewed below, though you don't have
 * to worry about them since they almost always generate from manual
 * assembler usage.

 * https://wiibrew.org/wiki/Paired_single

 * ================================================================ */

#include "00_basic_assembly_and_isa.h"

/* ================================================================ *
 *
 * The simplest instruction to consider is `add RT, RA, RB`, as all it
 * does is add two registers `RA` and `RB` together and place the
 * result in `RT`. Most instructions use general-purpose registers
 * (r0-r31), which hold 32 bits of data each on 32-bit PowerPC
 * processors and 64 bits on 64-bit processors. The Gamecube and Wii
 * both are 32-bit.
 * 
 * ================================================================ */

int addition(int a, int b) {
    // return a + b;
}

/* ================================================================ *
 *
 * Data can also be embedded in instructions themselves, such as
 * `addi`:
 * 
 * ================================================================ */

int addition_with_immediate(int a) {
    // return a + 7;
}

/* ================================================================ *
 *
 * Here are some other arithmetic operations that compile to 
 * simple assembly:
 * 
 * ================================================================ */

int subtraction(int a, int b) {
    // return a - b;
}

int multiplication(int a, int b) {
    // return a * b;
}

int division(int a, int b) {
    // return a / b;
}

/* ================================================================ *
 *
 * If you look up `li` (or hover over it in objdiff) you'll notice it
 * doesn't actually have its own instruction entry, instead being
 * labeled as an "extended mnemonic" of addi. These mnemonics exist
 * for developer convenience and don't change the functionality of the
 * instruction, so `li, r3, 7` is equivalent to `addi, r3, 0, 7`.
 *
 * Using addi as the example for this is a bit confusing because addi
 * also has a property where if you plug in 0 for the second register
 * argument, it's designed to interpret that as a literal 0 instead of
 * the register r0, which is why it's possible for it to double as a
 * "load immediate" instruction. Note how the mnemonic table and
 * objdiff write it as `addi, r3, 0, 7` instead of `addi, r3, r0, 7`.
 * 
 * ================================================================ */

int load() {
    // return 7;
}

/* ================================================================ *
 *
 * The first thing you should do if you get disoriented when looking
 * at a general-purpose register and you aren't sure what it's doing is
 * to verify whether it's a pointer or not. If you're a bit shaky on
 * how pointers work, now is a good time to watch a video or something
 * to refresh yourself. 
 * 
 * Basically, if you see a register that appears to the right of a
 * series of parenthesis in a store or load instruction, like the
 * 0x0(r3) and 0x4(r3) below, that register (until its data gets
 * written over by something else) is guaranteed to be a pointer. The
 * contents of r0, which is the number 7, is being written to the
 * memory address which is derived from the contents of r3, plus an
 * optional offset.
 * 
 * ================================================================ */

int store(int *a) {
    // *a = 7;
}

int store_offset(int *a) {
    // a[1] = 7;
}

/* ================================================================ *
 *
 * Floating point operations use bespoke floating-point registers
 * (f0-f31). They hold 64 bits each regardless of the processor being
 * 32-bit or 64-bit, so that both processors can use double-precision
 * floats. 
 *
 * In fact, every floating point instruction always operates on FPRs
 * as if they were doubles [1], as written in section 4.2.1 of the ISA
 * doc. The way this is achieved is that any time a single-precision
 * float has to be loaded from memory or stored to memory, it
 * implicitly does a conversion to and from double precision
 * respectively, which you can verify by looking up "lfs" and "stfs"
 * in the ISA.
 *
 * You don't have to worry about this in decomp since there still has
 * to be separate instructions for single versus double operations
 * (adds vs add), so you can easily identify the intended precision
 * for any floating-point related instruction. Just remember FPRs are
 * always 64 bits long.
 *
 * [1] minus the aforementioned "paired single" instructions since
 * they treat and operate on an FPR register as two single-precision
 * floats, which is how they gain the benefit of SIMD.
 * 
 * ================================================================ */

float addition_float(float a, float b) {
    // return a + b;
}

void addition_float_load_store(float *a, float *b, float *c) {
    // *c = *a + *b;
}

double addition_double(double a, double b) {
    // return a + b;
}

void addition_double_load_store(double *a, double *b, double *c) {
    // *c = *a + *b;
}

/* ================================================================ *
 *
 * This is an example of the use of *symbols* for a data load, which
 * is the same concept as how it was used to refer to a function in
 * `bl adder` in the "Compiling and linking" section of the decomp
 * intro. The finalized address for `some_int` has yet to be decided
 * until the linker is run, so the compiler inserts this symbol on the
 * `lwz` instruction. 
 * 
 * If you're using objdiff, you'll also notice `some_int` appears
 * below a tab labelled `.sdata`. This is the *data section* that the
 * value resides in, and more specifics on the different data section
 * types will be covered later.
 * 
 * ================================================================ */

int some_int = 21;

int load_int() {
    // return some_int;
}

/* ================================================================ *
 * 
 * End of chapter 0.
 * 
 * ================================================================ */
