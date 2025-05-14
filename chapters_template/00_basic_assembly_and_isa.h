// No problems require you to modify this header.

#ifndef CHAPTER_0_H
#define CHAPTER_0_H

int addition(int a, int b);
int addition_with_immediate(int a);
int load();
int store(int *a);
int store_offset(int *a);
float addition_float(float a, float b);
void addition_float_load_store(float *a, float *b, float *c);
double addition_double(double a, double b);
void addition_double_load_store(double *a, double *b, double *c);
int load_int();

extern int some_int;

#endif
