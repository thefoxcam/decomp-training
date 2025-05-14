#include "src/chapters_answers/00_basic_assembly_and_isa.h"
#include "src/chapters_answers/01_abi_basics.h"

void chapter_0() {
    int a = 1;
    int b = 1;
    float a_f = 1.0F;
    float b_f = 1.0F;
    float c_f = 1.0F;
    double a_d = 1.0F;
    double b_d = 1.0F;
    double c_d = 1.0F;

    addition(a, b);
    addition_with_immediate(a);
    load();
    store(&a);
    store_offset(&a);
    addition_float(a_f, b_f);
    addition_float_load_store(&a_f, &b_f, &c_f);
    addition_double(a_d, b_d);
    addition_double_load_store(&a_d, &b_d, &c_d);
    load_int();
}

void chapter_1() {
    int b = 3;

    abi_parameters(1, 2);
    abi_float_parameters(0.1F, 0.2F);
    abi_volatile_nonvolatile(4);
    abi_func_call();
    typical_stack_usage(2.0F);
    weird_func(5);
    call_weird_func(2, &b);
}

#if 0
void integers_64() {
    u64 a_64 = 1;
    u64 b_64 = 1;
    u32 a_32 = 1;

    example_64(a_64, b_64);
    aligned_64(a_32, b_64);
    add_64(a_64, b_64);
    add_64_downcast(a_64, b_64);
    sub_64(a_64, b_64);
    sub_64_downcast(a_64, b_64);
    mul_64(a_64, b_64);
    mul_64_downcast(a_64, b_64);
    div_64(a_64, b_64);
    mod_64(a_64, b_64);
    shl_64(a_64, b_64);
    shr_64(a_64, b_64);
    and_64(a_64, b_64);
    or_64(a_64, b_64);
    xor_64(a_64, b_64);
}
#endif

void sample_funcs() {
    chapter_0();
    chapter_1();
    // integers_64();
}
