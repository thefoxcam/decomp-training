#ifndef CHAPTER_1_H
#define CHAPTER_1_H

int abi_parameters(int a, int b);
float abi_float_parameters(float a, float b);
int abi_volatile_nonvolatile(int a);
void abi_func_call();
void typical_stack_usage(float a);
int weird_func(int a);
void call_weird_func(int a, int *b);

#endif
