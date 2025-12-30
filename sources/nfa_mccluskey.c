#include "nfa_mccluskey.h"

// Begin Nathan's function
regexp *nfa_mccluskey(nfa *A)
{
    /*
    // matrix creation to turn int into regexp elements
    regexp **matrix_reg = calloc(((A->transitions->nb_states + 2) * (A->transitions->nb_states + 2)), sizeof(*matrix_reg));

    int epsilon_idx = A->transitions->nb_labels;
    int qi = A->transitions->nb_states;
    int qf = A->transitions->nb_states + 1;

    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < A->transitions->nb_labels + 1; j++)
        {
            int c;
            int deb = A->transitions->intervals[(i * A->transitions->nb_labels) + j];
            int fin = A->transitions->intervals[(i * A->transitions->nb_labels) + j + 1];
            int transition_nb = A->transitions->intervals[(i * A->transitions->nb_labels) + j] - A->transitions->intervals[(i * A->transitions->nb_labels) + j + 1];

            for (int k = deb; k < fin; k++)
            {
                if (matrix_reg[(i * (A->transitions->nb_states + 2)) + A->transitions->to_array[k]] != NULL)
                {
                    if (j == epsilon_idx)
                    {
                        matrix_reg[(i * (A->transitions->nb_states + 2)) + A->transitions->to_array[k]] = reg_union(matrix_reg[(i * (A->transitions->nb_states + 2)) + A->transitions->to_array[k]], reg_epsilon());
                    }
                    else
                    {
                        matrix_reg[(i * (A->transitions->nb_states + 2)) + A->transitions->to_array[k]] = reg_union(matrix_reg[(i * (A->transitions->nb_states + 2)) + A->transitions->to_array[k]], reg_letter(A->letter_names[j]));
                    }
                }
                else
                {
                    if (j == epsilon_idx)
                    {
                        matrix_reg[(i * (A->transitions->nb_states + 2)) + A->transitions->to_array[k]] = reg_epsilon();
                    }
                    else
                    {
                        matrix_reg[(i * (A->transitions->nb_states + 2)) + A->transitions->to_array[k]] = reg_letter(A->letter_names[j]);
                    }
                }
            }
        }
    }

    // qi part
    for (int i = 0; i < A->nb_initials; i++)
    {
        if (matrix_reg[(qi * (A->transitions->nb_states + 2)) + A->initials[i]] == NULL)
        {
            matrix_reg[qi * (A->transitions->nb_states + 2) + A->initials[i]] = reg_epsilon();
        }
        else
        {
            matrix_reg[(qi * (A->transitions->nb_states + 2)) + A->initials[i]] = reg_union(matrix_reg[(qi * (A->transitions->nb_states + 2)) + A->initials[i]], reg_epsilon());
        }
    }

    // qf part
    for (int i = 0; i < A->nb_finals; i++)
    {
        if (matrix_reg[(A->finals[i] * (A->transitions->nb_states + 2)) + qf] == NULL)
        {
            matrix_reg[(A->finals[i] * (A->transitions->nb_states + 2)) + qf] = reg_epsilon();
        }
        else
        {
            matrix_reg[(A->finals[i] * (A->transitions->nb_states + 2)) + qf] = reg_union(matrix_reg[(A->finals[i] * (A->transitions->nb_states + 2)) + qf], reg_epsilon());
        }
    }
    */
    return NULL;
}
// End Nathan's function
