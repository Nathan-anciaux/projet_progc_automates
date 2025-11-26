#include "type_boolarray.h"

barray *create_barray(int size)
//
{
    barray *b = calloc(1, sizeof(*b));
    b->size = size;
    if (b->size % 8 == 0)
    {
        b->size_array = b->size / 8;
    }
    b->size_array = (b->size / 8) + 1;
    b->array = calloc(b->size_array, sizeof(*(b->array)));
    return b;
}

int getsize_barray(barray *b)
{
    return b->size;
}

void delete_barray(barray *b)
{
    free(b->array);
    free(b);
    return;
}

void settrue_barray(barray *b, int index)
{
    b->array[index / 8] |= (1 << (index % 8));
    return;
}

void setfalse_barray(barray *b, int index)
{
    b->array[index / 8] &= (~(1 << (index % 8)));
    return;
}

bool getval_barray(barray *, int)
{

    return false;
}

barray *or_barray(barray *, barray *)
{
    return NULL;
}
barray *and_barray(barray *, barray *)
{
    return NULL;
}

barray *copy_barray(barray *)
{
    return NULL;
}
