#ifndef BOOLARRAY_H_
#define BOOLARRAY_H_

#include "system_tools.h"

typedef struct
{
    uchar *array;
    int size;
    int size_array;
} barray;

barray *create_barray(int size);

int getsize_barray(barray *ba);

void delete_barray(barray *ba);

void settrue_barray(barray *ba, int cell);

void setfalse_barray(barray *ba, int cell);

bool getval_barray(barray *ba, int cell);

barray *or_barray(barray *ba1, barray *ba2);

barray *and_barray(barray *ba1, barray *ba2);

barray *copy_barray(barray *ba);

#endif
