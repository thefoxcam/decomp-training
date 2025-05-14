#include "src/chapters_answers/00_basic_assembly_and_isa.h"

int addition(int a, int b) {
    return a + b;
}

int addition_with_immediate(int a) {
    return a + 7;
}

int load() {
    return 7;
}

int store(int *a) {
    *a = 7;
}

int store_offset(int *a) {
    a[1] = 7;
}

float addition_float(float a, float b) {
    return a + b;
}

void addition_float_load_store(float *a, float *b, float *c) {
    *c = *a + *b;
}

double addition_double(double a, double b) {
    return a + b;
}

void addition_double_load_store(double *a, double *b, double *c) {
    *c = *a + *b;
}

int some_int = 21;

int load_int() {
    return some_int;
}
