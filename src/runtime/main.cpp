#include <Common.h>
extern "C"
{
  #include "sample_functions.h"
}

void *operator new( size_t size )
{
    return NULL;
}

void operator delete( void *block ) {}

struct __Sample
{
    __Sample( ) {}
    virtual ~__Sample( ) {}

    int _0;
};

static __Sample __sample;

int main( void )
{
    __sample._0 = 0xf3;
    sample_funcs();
    return 0;
}
