#include "src/runtime/platform.h"

u64 example_64(u64 a, u64 b) {
    return a + b;
}

u64 aligned_64(u32 a, u64 b) {
    return b + 5;
}

u64 add_64(u64 a, u64 b) {
    return a + b;
}

u32 add_64_downcast(u64 a, u64 b) {
    return a + b;
}

u64 sub_64(u64 a, u64 b) {
    return a - b;
}

u32 sub_64_downcast(u64 a, u64 b) {
    return a - b;
}

u64 mul_64(u64 a, u64 b) {
    return a * b;
}

u32 mul_64_downcast(u64 a, u64 b) {
    return a * b;
}

u64 div_64(u64 a, u64 b) {
    return a / b;
}

u64 mod_64(u64 a, u64 b) {
    return a % b;
}

u64 shl_64(u64 a, u64 b) {
    return a << b;
}

u64 shr_64(u64 a, u64 b) {
    return a >> b;
}

u64 and_64(u64 a, u64 b) {
    return a & b;
}

u64 or_64(u64 a, u64 b) {
    return a | b;
}

u64 xor_64(u64 a, u64 b) {
    return a ^ b;
}

#if 0
// TODO(fox): This should probably be combined with the branching section

int abi_function_6(int a) {
    switch (a) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 3;
        case 4:
            return 4;
        case 5:
            return 5;
    }
    return 0;
}

int abi_function_7(int a) {
    some_func();
    switch (a) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 3;
        case 4:
            return 4;
        case 5:
            return 5;
    }
    return 0;
}
#endif
