/* ================================================================ *
 *
 **** CHATPER 1 - ABI BASICS 
 *
 * This section will likely be dense and boring, but it will clear up
 * a lot of uncertainties as to what is going on when you look at a
 * block of assembly. I'll start to not provide the solutions on some
 * of the functions, so try to start matching the functions when
 * they're empty.
 *
 * While the ISA describes many of the fundamental features and
 * aspects of a target architecture, it doesn't prescribe how a given
 * program should behave on a given target operating system. That
 * instead is delegated to a specification called the *application
 * binary interface* (ABI), which theoretically allows a program to
 * interoperate with other programs on the system and be able to run
 * on any computer with the OS and hardware it was compiled to run on.
 * The compiler ultimately gets to define the "final ABI" that the
 * program compiled on it uses, often being layered on top of a more
 * general OS ABI, so you could hear phrases like "the Metrowerks C++
 * ABI" or "the Linux ABI."
 * 
 * For example, every library and function compiled with a given
 * compiler for the GC/Wii "agrees" that it will use a specific
 * register to return values if the function returns something (r3),
 * which you'll see in the first training function. 
 *
 * An ABI similar to the what Metrowerks uses that is useful to 
 * reference is the System V PowerPC ABI, which can be found here:
 *
 * http://refspecs.linux-foundation.org/elf/elfspec_ppc.pdf
 *
 * In addition, since the GC/Wii is an embedded system, it also obeys
 * a superset of the ABI known as an *embedded* application binary
 * interface, or EABI, which can be viewed here:
 *
 * https://files.decomp.dev/E500ABIUG.pdf
 *
 * References to the general ABI will use the notation (ABI page #-#), 
 * and the EABI will use (EABI page #-#), 
 *
 * ================================================================ */

#include "stuff.h"

/* ================================================================ *
 *
 * Registers and parameters (Function Calling Sequence, ABI page 3-14)
 *
 * As you may be familiar with from a computer architecture or
 * assembly class, the CPU has to move any data it wants to operate on
 * from main memory (RAM) into its own registers to be able to access
 * it. Thus, an optimization most ABIs utilize to reduce the amount of
 * times a given function has to access RAM is to allow registers
 * themselves to be used as both function arguments and return values. 
 *
 * In the case of non-float values, up to eight general-purpose
 * registers can used to pass arguments to a function, starting at
 * r3 and ending at r10. Additionally, r3 acts a return register
 * that a callsite of the function can read from. 
 *
 * For example, to write a function add() that adds two integer
 * arguments together and returns the result, you simply have to add
 * r3 and r4 and store the result in r3.
 *
 * ================================================================ */

int abi_parameters(int a, int b) {
}

/* ================================================================ *
 * 
 * For floats, up to eight floating-point registers can be used,
 * starting at f1 and ending at f8. f1 also acts as the return register. 
 *
 * ================================================================ */

float abi_float_parameters(float a, float b) {
}

/* ================================================================ *
 *
 * Volatile and non-volatile registers (Registers, ABI page 3-14)
 * 
 * The sets of registers used for function passing (r3-r8, f1-f8) are
 * also known as *volatile* registers, because whenever you branch
 * into a new function, the ABI allows the values in those registers
 * to get overwritten. In other words, any time you step over a "b
 * some_func", you must assume that all volatile registers have
 * effectively been destroyed (or modified in the case of return
 * registers). Thus the concept of "non-volatile" registers becomes
 * useful, which allows us to preserve data between registers that
 * cross callsites without having to read/write from main memory.
 *
 * The simplest example of this behavior can be seen in the `mr r31,
 * r3` below, where r3 is moved to the non-volatile register r31,
 * since r3 can get overwritten by some_func [1]. r31 is then passed back
 * to r3 to be used as the return register. The other instructions,
 * which are related to the stack, will be explained next.
 *
 * [1] some_func is defined outside this TU, and its implementation is
 * not important for this discussion.
 *
 * ================================================================ */

int abi_volatile_nonvolatile(int a) {
    // some_func();
    // return a;
}

/* ================================================================ *
 *
 * Function prologues/epilogues, the stack, and the link register
 * (Function Prologue and Epilogue, ABI page 3-34)
 * (The Stack Frame, ABI page 3-17)
 * 
 * You may have noticed a pattern among the functions listed so far in
 * that they all end in the "blr" opcode, and many of them contain a
 * "mflr" towards the top and a "mtlr" towards the bottom. These
 * opcodes all concern a special register called the *link register*,
 * which holds the memory address of the previous function that called
 * the function you're currently in. This was also explained in the
 * intro doc, but to reiterate again to help make it stick in your
 * brain more:
 *
 * | // some_func
 * | 0x80103F18 | blr
 * | 
 * | // a bunch of code
 * | 
 * | // abi_function_4
 * | 0x802D8910 | mflr r0
 * | 0x802D8914 | stw r0, 0x4(r1)
 * | 0x802D8918 | stwu r1, -0x8(r1)
 * | 0x802D891C | bl 0x80103F14 // address of some_func
 * | 0x802D8920 | lwz r0, 0xc(r1)
 * | 0x802D8924 | addi r1, r1, 0x8
 * | 0x802D8918 | mtlr r0
 * | 0x802D891C | blr
 *
 * (note: addresses may be different from the actual executable)
 *
 * When the "bl 0x80103F14" instruction gets executed, the link
 * register (LR) is automatically set with the address of the next
 * instruction at 0x802D8920, which is "lwz r0, 0xc(r1)". Then once it
 * hits the blr in "some_func" at 0x80103F18, it branches to the value
 * at the LR (0x802D8920) and resumes where it left off in
 * "abi_function_4."
 *
 * However you may be wondering, if the LR gets overwritten by the "bl
 * 0x80103F14" in "abi_function_4," then what will happen to the LR
 * that "abi_function_4" is currently holding? It won't be able to
 * return to the function that's calling itself when it executes its
 * "blr" if its LR gets overwritten! Luckily, that problem is exactly
 * what all of the other instructions are addressing, which are a part
 * of what is known as the "prologue" and "epilogue" that are executed
 * at the beginning and end of a function respectively. 

 * In the prologue, "mflr" moves the address in the LR to a register
 * r0, which is then stored in what is known as the "stack" in the
 * "stw". Note that r1 is a special register which holds the stack
 * pointer, and its current value is required to be decremented and
 * placed on the stack (in the "stwu"). Then in the epilogue, that
 * address is loaded out from the stack back into r0 and moved back
 * into the LR in the "lwz" and "mtlr" instructions, which allows the
 * "blr" to successfully return to "abi_function_4"'s caller. [1]
 *
 * The reason "mflr" and "mtlr" don't show up in every function, as
 * you may be able to guess, is that a function doesn't need to save
 * and restore the LR unless it actually needs to (i.e. it calls a
 * function), which is why some_func doesn't have them. 
 *
 * [1] You can read more about the stack setup on section 3-34 of the
 * ABI doc.
 *
 * ================================================================ */

void abi_func_call(float a) {
    // some_func();
}

/* ================================================================ *
 *
 * Besides saving/restoring the LR in the prologue/epilogue, the stack
 * will also get used by normal code for various reasons when
 * registers aren't enough to represent the data. Normally when you
 * declare variables on "the stack" in C, the compiler will try to
 * avoid actually implementing a stack and simply use registers.
 * However one case where the compiler won't do that is if you declare
 * a struct on the stack and you pass its address to a function, in
 * which case it always implements a real stack. Typically you'll see
 * this with math-related structs like a Vec3 or Vec4 or Matrix struct.
 *
 * ================================================================ */

void typical_stack_usage(float a) {
    // Vec3 pos;
    // pos.x = a;
    // pos.y = a;
    // pos.z = a;
    // some_func_vec3(&pos);
}

/* ================================================================ *
 *
 * Here is your first "real" problem without an explanation and
 * without the return or input types provided (you'll have to modify
 * the accompanying .h file as well). It's a bit tricky, but with the
 * knowledge you now have, you should be equipped to tackle and
 * understand what at first glance looks like a strange peculiarity.
 * Uncomment the functions and pretend that "weird_func" is in another
 * TU; don't remove the pragma statements (it's the same trick from
 * the intro article to make it not automatically get inlined by the
 * compiler).
 *
 * View the solution here if you get stuck or figure it out; it
 * contains an important explanation as well:
 *
 * https://wiki.decomp.dev/en/resources/decomp-training-answers/chapter_01
 *
 * ================================================================ */

#pragma push
#pragma dont_inline on

/*
??? weird_func(???) {
    ??? 
}

??? call_weird_func(???) {
    ??? 
}
*/

#pragma pop

/* ================================================================ *
 * 
 * End of chapter 1.
 * 
 * ================================================================ */


