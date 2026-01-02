#include "nfa_mccluskey.h"

// Begin Nathan's function

int index_in_matrix_reg(nfa *A, int i, int j)
{
    return (i * (A->transitions->nb_states + 2)) + j;
}

regexp *nfa_mccluskey(nfa *A)
{

    // matrix creation to turn int into regexp elements
    regexp **matrix_reg = calloc(((A->transitions->nb_states + 2) * (A->transitions->nb_states + 2)), sizeof(*matrix_reg));

    int epsilon_idx = -1;
    if (A->eps_trans)
    {
        epsilon_idx = A->transitions->nb_labels - 1;
    }
    int qi = A->transitions->nb_states;
    int qf = A->transitions->nb_states + 1;

    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < A->transitions->nb_labels; j++)
        {
            int deb = A->transitions->intervals[(i * A->transitions->nb_labels) + j];
            int fin = A->transitions->intervals[(i * A->transitions->nb_labels) + j + 1];

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

    // algorithme Brzozowski-McCluskey
    for (int k = 0; k < A->transitions->nb_states; k++)
    {
        for (int i = 0; i < A->transitions->nb_states + 2; i++)
        {
            for (int j = 0; j < A->transitions->nb_states + 2; j++)
            {
                if (matrix_reg[index_in_matrix_reg(A, i, k)] != NULL && matrix_reg[index_in_matrix_reg(A, k, j)] != NULL)
                {
                    if (i != k && j != k)
                    {
                        regexp *kk;
                        if (matrix_reg[index_in_matrix_reg(A, k, k)] != NULL)
                        {
                            kk = matrix_reg[index_in_matrix_reg(A, k, k)];
                        }
                        else
                        {
                            kk = reg_epsilon();
                        }
                        regexp *k_kleene = reg_star(kk);
                        regexp *transfo_2 = reg_concat(matrix_reg[index_in_matrix_reg(A, i, k)], reg_concat(k_kleene, matrix_reg[index_in_matrix_reg(A, k, j)]));

                        matrix_reg[index_in_matrix_reg(A, i, j)] = reg_union(matrix_reg[index_in_matrix_reg(A, i, j)], transfo_2);
                    }
                }
            }
        }
    }

    return matrix_reg[index_in_matrix_reg(A, qi, qf)];
}
// End Nathan's function
