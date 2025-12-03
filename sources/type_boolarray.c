#include "type_boolarray.h"

// Begin Nathan's Functions
barray *create_barray(int size)
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
// End Nathan's Functions

// Begin Arthur's Functions
bool getval_barray(barray *b, int index)
{
    return b->array[index / 8] == (1 << (index % 8));
}

barray *or_barray(barray *b, barray *b2)
{
    if (b->size == b2->size)
    {
        return NULL;
    }
    barray *b3 = malloc(sizeof(barray));
    b3->size = b->size;
    b3->size = b->size_array;
    for (int i = 0; i < b->size_array; i++)
    {
        b3->array[i] = b->array[i] | b2->array[i];
    }
    return b3;
}
barray *and_barray(barray *b, barray *b2)
{
    if (b->size == b2->size)
    {
        return NULL;
    }
    barray *b3 = malloc(sizeof(barray));
    b3->size = b->size;
    b3->size = b->size_array;
    for (int i = 0; i < b->size_array; i++)
    {
        b3->array[i] = b->array[i] & b2->array[i];
    }
    return b3;
}

barray *copy_barray(barray *b)
{
    barray *b2 = malloc(sizeof(barray));
    b2->size = b->size;
    b2->size_array = b->size_array;
    for (int i = 0; i < b->size_array; i++)
    {
        b2->array[i] = b->array[i];
    }
    return b2;
}

// Note

// Pour la fonction create_barray(int size) :
// barray *b = calloc(1, sizeof(b)); // Pas de * pour sizeof(b) sinon on réserve la taille d'une adresse mémoire ( euu c'est pas l'inverse ? regarde ce que j'ai fait en haut ça foncionne : nathans) / Arthur : Autant pour moi, ça fonctionne mais c'est une allocation mémoire du futur
// Mettre b->size_array = (b->size / 8) + 1; avant le if sinon on ecrase toujours la valeur de b->size_array et le if ne sert pas.
// b->array = calloc(b->size_array, sizeof((b->array))); // Pas de * à sizeof((b->array)
// End Arthur's Functions
