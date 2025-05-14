#include "src/runtime/stuff.h"

int abi_parameters(int a, int b) {
    return a + b;
}

float abi_float_parameters(float a, float b) {
    return a + b;
}

int abi_volatile_nonvolatile(int a) {
    some_func();
    return a;
}

void abi_func_call() {
    some_func();
}

void typical_stack_usage(float a) {
    Vec3 pos;
    pos.x = a;
    pos.y = a;
    pos.z = a;
    some_func_vec3(&pos);
}

int weird_func(int a) {
    return a;
}

void call_weird_func(int a, int *b) {
    *b = weird_func(a);
}

