#include "nfa.h"
#include "system_printing.h"
#include <string.h>

void nfa_reset_state_names(nfa *A)
{
    if (!A || !A->state_names)
    {
        return;
    }

    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        // printf("%d %s\n", A->transitions->nb_states, A->state_names[i]);
        free(A->state_names[i]);
    }
    free(A->state_names);
    A->state_names = NULL;
}

/*********************************/
/* Initialisation et suppression */
/*********************************/
// Begin Nathan's functions
nfa *create_emptylang(void)
{
    // nfa allocation
    nfa *n = calloc(1, sizeof(*n));

    // graph part
    n->transitions = calloc(1, sizeof(*(n->transitions)));
    n->transitions->intervals = calloc(1, sizeof(*(n->transitions->intervals)));
    n->transitions->to_array = NULL;

    // nfa link with ngraph
    n->nb_states = n->transitions->nb_states;

    // nfa part
    n->finals = NULL;
    n->initials = NULL;
    n->letter_names = NULL;
    n->state_names = NULL;

    return n;
}

nfa *create_sing_epsilon(void)
{
    // nfa allocation
    nfa *n = malloc(sizeof(*n));

    // graph part
    n->transitions = malloc(sizeof(*(n->transitions)));
    n->transitions->nb_labels = 0;
    n->transitions->nb_states = 1;

    n->transitions->intervals = calloc(2, sizeof(*(n->transitions->intervals)));
    n->transitions->to_array = NULL;

    // mfa link with ngraph
    n->nb_states = n->transitions->nb_states;

    // nfa part
    n->nb_letters = 0;
    n->eps_trans = true;
    n->nb_initials = 1;
    n->nb_finals = 1;

    n->finals = malloc(n->nb_finals * sizeof(*(n->finals)));
    n->finals[0] = 0;
    n->initials = malloc(n->nb_initials * sizeof(*(n->initials)));
    n->initials[0] = 0;
    n->letter_names = NULL;
    n->state_names = NULL;

    return n;
}
// End Nathan's functions

// Begin Arthur's functions
nfa *create_sing_letter(char letter)
{
    nfa *A = malloc(sizeof(nfa));
    A->nb_states = 2;
    A->nb_letters = 1;
    A->eps_trans = false;
    A->transitions = malloc(sizeof(ngraph));
    A->transitions->nb_states = 2;
    A->transitions->nb_labels = 1;
    A->transitions->to_array = malloc(sizeof(int));
    A->transitions->to_array[0] = 1;
    A->transitions->intervals = malloc(sizeof(int) * ((A->nb_states * A->nb_letters) + 1));
    A->transitions->intervals[0] = 0;
    A->transitions->intervals[1] = 1;
    A->transitions->intervals[2] = 1;
    A->initials = malloc(sizeof(int));
    A->initials[0] = 0;
    A->nb_initials = 1;
    A->finals = malloc(sizeof(int));
    A->finals[0] = 1;
    A->nb_finals = 1;
    A->letter_names = malloc(sizeof(char));
    A->letter_names[0] = letter;
    A->state_names = malloc(sizeof(char *) * 2);
    A->state_names[0] = strdup("Initial state");
    A->state_names[1] = strdup("Recognized letter");
    return A;
}

void nfa_delete(nfa *A)
{
    free(A->transitions->to_array);
    free(A->transitions->intervals);
    free(A->transitions);
    free(A->initials);
    free(A->finals);
    free(A->letter_names);
    if (A->state_names != NULL)
    {
        for (int i = 0; i < A->nb_states; i++)
        {
            free(A->state_names[i]);
        }
        free(A->state_names);
    }
    free(A);
    return;
}
// End Arthur's functions
/***********************************/
/* Opérations simples sur les NFAs */
/***********************************/

// Begin Nathan's function
nfa *nfa_copy(nfa *A)
{
    nfa *B = malloc(sizeof(*B));

    B->nb_states = A->nb_states;
    B->eps_trans = A->eps_trans;

    // graph part
    B->transitions = malloc(sizeof(*(B->transitions)));

    B->transitions->nb_states = A->transitions->nb_states;
    B->transitions->nb_labels = A->transitions->nb_labels;

    B->transitions->to_array = malloc(B->transitions->nb_states * sizeof(*(B->transitions->to_array)));

    B->transitions->intervals = malloc(((B->transitions->nb_states * B->transitions->nb_labels) + 1) * sizeof(*(B->transitions->intervals)));

    for (int i = 0; i < B->transitions->nb_states; i++)
    {
        B->transitions->to_array[i] = A->transitions->to_array[i];
    }
    for (int i = 0; i < (B->transitions->nb_states * B->transitions->nb_labels) + 1; i++)
    {
        B->transitions->intervals[i] = A->transitions->intervals[i];
    }

    // initial tab
    B->nb_initials = A->nb_initials;
    B->initials = malloc(B->nb_initials * sizeof(*(B->initials)));
    for (int i = 0; i < B->nb_initials; i++)
    {
        B->initials[i] = A->initials[i];
    }

    // final tab
    B->nb_finals = A->nb_finals;
    B->finals = malloc(B->nb_finals * sizeof(*(B->finals)));
    for (int i = 0; i < B->nb_finals; i++)
    {
        B->finals[i] = A->finals[i];
    }

    // letter tab
    B->nb_letters = A->nb_letters;
    B->letter_names = malloc(B->nb_letters * sizeof(*(B->letter_names)));
    for (int i = 0; i < B->nb_letters; i++)
    {
        B->letter_names[i] = A->letter_names[i];
    }

    // state name part
    if (A->state_names == NULL)
    {
        B->state_names = NULL;
    }
    else
    {
        B->state_names = malloc(A->nb_states * sizeof(*(B->state_names)));
        for (int i = 0; i < B->nb_states; i++)
        {
            B->state_names[i] = strdup(A->state_names[i]);
        }
    }

    return B;
}

nfa *nfa_union(nfa *A, nfa *B)
{
    nfa *C = malloc(sizeof(*C));

    // letter tab
    C->letter_names = malloc((A->nb_letters + B->nb_letters) * sizeof(*(C->letter_names)));
    for (int i = 0; i < A->nb_letters; i++)
    {
        C->letter_names[i] = A->letter_names[i];
    }
    int u = 0;
    int v = A->nb_letters;
    while (u < B->nb_letters)
    {
        bool is_letter = false;
        for (int j = 0; j < A->nb_letters; j++)
        {
            if (B->letter_names[u] == A->letter_names[j])
            {
                is_letter = true;
            }
        }
        if (is_letter == false)
        {
            C->letter_names[v] = B->letter_names[u];
            v++;
            u++;
        }
        else
        {
            u++;
        }
    }
    C->nb_letters = v;

    // initial tab
    C->nb_initials = A->nb_initials + B->nb_initials;
    C->initials = malloc(C->nb_initials * sizeof(*(C->initials)));
    for (int i = 0; i < A->nb_initials; i++)
    {
        C->initials[i] = A->initials[i];
    }
    for (int i = A->nb_initials; i < C->nb_initials; i++)
    {
        C->initials[i] = B->initials[i - A->nb_initials] + A->nb_states;
    }

    // final tab
    C->nb_finals = A->nb_finals + B->nb_finals;
    C->finals = malloc(C->nb_finals * sizeof(*(C->finals)));
    for (int i = 0; i < A->nb_finals; i++)
    {
        C->finals[i] = A->finals[i];
    }
    for (int i = A->nb_finals; i < C->nb_finals; i++)
    {
        C->finals[i] = B->finals[i - A->nb_finals] + A->nb_states;
    }

    C->nb_states = A->nb_states + B->nb_states;
    C->eps_trans = A->eps_trans || B->eps_trans;

    // state name part
    if ((A->state_names == NULL) || (B->state_names == NULL))
    {
        C->state_names = NULL;
    }
    else
    {
        C->state_names = malloc(C->nb_states * sizeof(*(C->state_names)));
        for (int j = 0; j < A->nb_states; j++)
        {
            C->state_names[j] = strdup(A->state_names[j]);
        }
        for (int j = 0; j < B->nb_states; j++)
        {
            C->state_names[j + A->nb_states] = strdup(B->state_names[j]);
        }
    }

    // graph part
    C->transitions = malloc(sizeof(*(C->transitions)));

    C->transitions->nb_states = A->transitions->nb_states + B->transitions->nb_states;
    C->transitions->nb_labels = C->nb_letters;

    C->transitions->intervals = malloc(((C->transitions->nb_states * C->transitions->nb_labels) + 1) * sizeof(*(C->transitions->intervals)));

    // tab creation to help interval tab creation
    int *ind_A = malloc(C->transitions->nb_labels * sizeof(*ind_A));
    int *ind_B = malloc(C->transitions->nb_labels * sizeof(*ind_B));

    for (int i = 0; i < C->transitions->nb_labels; i++)
    {
        ind_A[i] = -1;
        ind_B[i] = -1;

        for (int k = 0; k < A->transitions->nb_labels; k++)
        {
            if (A->letter_names[k] == C->letter_names[i])
            {
                ind_A[i] = k;
                break;
            }
        }
        for (int k = 0; k < B->transitions->nb_labels; k++)
        {
            if (B->letter_names[k] == C->letter_names[i])
            {
                ind_B[i] = k;
                break;
            }
        }
    }

    int index_in_interval = 0;
    int write_in_interval = 0;
    // fill interval tab
    C->transitions->intervals[0] = 0;
    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int count = 0;
            if (ind_A[j] != -1)
            {
                int a = ind_A[j];
                index_in_interval++;
                count = A->transitions->intervals[(i * A->nb_letters) + a + 1] - A->transitions->intervals[(i * A->nb_letters) + a];
                write_in_interval = write_in_interval + count;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
            else
            {
                index_in_interval++;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
        }
    }
    for (int i = A->transitions->nb_states; i < C->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int count = 0;
            if (ind_B[j] != -1)
            {
                int b = ind_B[j];
                index_in_interval++;
                count = B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + b + 1] - B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + b];
                write_in_interval = write_in_interval + count;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
            else
            {
                index_in_interval++;
                C->transitions->intervals[index_in_interval] = write_in_interval;
            }
        }
    }

    // fill to_array
    int size_to_array = C->transitions->intervals[(C->transitions->nb_labels * C->transitions->nb_states)];
    C->transitions->to_array = malloc(size_to_array * sizeof(*(C->transitions->to_array)));

    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            if (ind_A[j] != -1)
            {
                int l_a = ind_A[j];

                int deb = A->transitions->intervals[(i * A->nb_letters) + l_a];
                int fin = A->transitions->intervals[((i * A->nb_letters) + l_a) + 1];

                int write_pos = C->transitions->intervals[(i * C->nb_letters) + j];

                for (int k = deb; k < fin; k++)
                {
                    C->transitions->to_array[write_pos] = A->transitions->to_array[k];
                    write_pos++;
                }
            }
        }
    }
    for (int i = A->transitions->nb_states; i < C->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            if (ind_B[j] != -1)
            {
                int l_b = ind_B[j];

                int deb = B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + l_b];
                int fin = B->transitions->intervals[(((i - A->transitions->nb_states) * B->nb_letters) + l_b) + 1];

                int write_pos = C->transitions->intervals[(i * C->nb_letters) + j];

                for (int k = deb; k < fin; k++)
                {
                    C->transitions->to_array[write_pos] = B->transitions->to_array[k] + A->nb_states;
                    write_pos++;
                }
            }
        }
    }

    free(ind_A);
    free(ind_B);
    return C;
}

bool is_state_final(nfa *A, int state)
{
    for (int i = 0; i < A->nb_finals; i++)
    {
        if (state == A->finals[i])
        {
            return true;
        }
    }
    return false;
}

nfa *nfa_concat(nfa *A, nfa *B)
{
    nfa *C = malloc(sizeof(*C));

    // letter tab
    C->letter_names = malloc((A->nb_letters + B->nb_letters) * sizeof(*(C->letter_names)));
    for (int i = 0; i < A->nb_letters; i++)
    {
        C->letter_names[i] = A->letter_names[i];
    }
    int u = 0;
    int v = A->nb_letters;
    while (u < B->nb_letters)
    {
        bool is_letter = false;
        for (int j = 0; j < A->nb_letters; j++)
        {
            if (B->letter_names[u] == A->letter_names[j])
            {
                is_letter = true;
            }
        }
        if (is_letter == false)
        {
            C->letter_names[v] = B->letter_names[u];
            v++;
            u++;
        }
        else
        {
            u++;
        }
    }
    C->nb_letters = v;

    // initial tab
    C->nb_initials = A->nb_initials;
    C->initials = malloc(C->nb_initials * sizeof(*(C->initials)));
    for (int i = 0; i < A->nb_initials; i++)
    {
        C->initials[i] = A->initials[i];
    }

    // final tab
    C->nb_finals = B->nb_finals;
    C->finals = malloc(C->nb_finals * sizeof(*(C->finals)));
    for (int i = 0; i < B->nb_finals; i++)
    {
        C->finals[i] = B->finals[i] + A->nb_states;
    }

    C->nb_states = A->nb_states + B->nb_states;
    C->eps_trans = true;

    // state name part
    if ((A->state_names == NULL) || (B->state_names == NULL))
    {
        C->state_names = NULL;
    }
    else
    {
        C->state_names = malloc(C->nb_states * sizeof(*(C->state_names)));
        for (int j = 0; j < A->nb_states; j++)
        {
            C->state_names[j] = strdup(A->state_names[j]);
        }
        for (int j = 0; j < B->nb_states; j++)
        {
            C->state_names[j + A->nb_states] = strdup(B->state_names[j]);
        }
    }

    // graph part
    C->transitions = malloc(sizeof(*(C->transitions)));

    C->transitions->nb_states = A->transitions->nb_states + B->transitions->nb_states;
    C->transitions->nb_labels = C->nb_letters + 1;
    int epsilon_index = C->nb_letters;

    C->transitions->intervals = malloc(((C->transitions->nb_states * C->transitions->nb_labels) + 1) * sizeof(*(C->transitions->intervals)));

    // tab creation to help interval tab creation
    int *ind_A = malloc(C->transitions->nb_labels * sizeof(*ind_A));
    int *ind_B = malloc(C->transitions->nb_labels * sizeof(*ind_B));

    for (int i = 0; i < C->transitions->nb_labels; i++)
    {
        ind_A[i] = -1;
        ind_B[i] = -1;

        for (int k = 0; k < A->transitions->nb_labels; k++)
        {
            if (A->letter_names[k] == C->letter_names[i])
            {
                ind_A[i] = k;
                break;
            }
        }
        for (int k = 0; k < B->transitions->nb_labels; k++)
        {
            if (B->letter_names[k] == C->letter_names[i])
            {
                ind_B[i] = k;
                break;
            }
        }
    }

    int index_in_interval = 0;
    int write_in_interval = 0;
    // fill interval tab
    C->transitions->intervals[0] = 0;
    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int count = 0;
            if (j == epsilon_index && is_state_final(A, i))
            {
                write_in_interval = write_in_interval + B->nb_initials;
            }
            if (ind_A[j] != -1)
            {
                int a = ind_A[j];
                count = A->transitions->intervals[(i * A->nb_letters) + a + 1] - A->transitions->intervals[(i * A->nb_letters) + a];
                write_in_interval = write_in_interval + count;
            }

            index_in_interval++;
            C->transitions->intervals[index_in_interval] = write_in_interval;
        }
    }
    for (int i = A->transitions->nb_states; i < C->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int count = 0;
            if (ind_B[j] != -1)
            {
                int b = ind_B[j];
                count = B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + b + 1] - B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + b];
                write_in_interval = write_in_interval + count;
            }
            index_in_interval++;
            C->transitions->intervals[index_in_interval] = write_in_interval;
        }
    }

    // fill to_array
    int size_to_array = C->transitions->intervals[(C->transitions->nb_labels * C->transitions->nb_states)];
    C->transitions->to_array = malloc(size_to_array * sizeof(*(C->transitions->to_array)));

    for (int i = 0; i < A->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int write_pos = C->transitions->intervals[(i * C->transitions->nb_labels) + j];
            if (ind_A[j] != -1)
            {
                int l_a = ind_A[j];

                int deb = A->transitions->intervals[(i * A->nb_letters) + l_a];
                int fin = A->transitions->intervals[((i * A->nb_letters) + l_a) + 1];

                for (int k = deb; k < fin; k++)
                {
                    C->transitions->to_array[write_pos] = A->transitions->to_array[k];
                    write_pos++;
                }
            }
            if (j == epsilon_index && is_state_final(A, i))
            {
                for (int k = 0; k < B->nb_initials; k++)
                {
                    C->transitions->to_array[write_pos] = B->initials[k] + A->nb_states;
                    write_pos++;
                }
            }
        }
    }
    for (int i = A->transitions->nb_states; i < C->transitions->nb_states; i++)
    {
        for (int j = 0; j < C->transitions->nb_labels; j++)
        {
            int write_pos = C->transitions->intervals[(i * C->transitions->nb_labels) + j];
            if (ind_B[j] != -1)
            {
                int l_b = ind_B[j];

                int deb = B->transitions->intervals[((i - A->transitions->nb_states) * B->nb_letters) + l_b];
                int fin = B->transitions->intervals[(((i - A->transitions->nb_states) * B->nb_letters) + l_b) + 1];

                for (int k = deb; k < fin; k++)
                {
                    C->transitions->to_array[write_pos] = B->transitions->to_array[k] + A->nb_states;
                    write_pos++;
                }
            }
        }
    }

    free(ind_A);
    free(ind_B);
    return C;
}

nfa *nfa_star(nfa *A)
{
    nfa *C = malloc(sizeof(*C));

    // letter tab
    C->letter_names = malloc(A->nb_letters * sizeof(*(C->letter_names)));
    C->nb_letters = A->nb_letters;
    for (int i = 0; i < C->nb_letters; i++)
    {
        C->letter_names[i] = A->letter_names[i];
    }

    // dim
    C->nb_states = A->nb_states + 2; // final and initial

    C->eps_trans = true;

    // initial tab
    C->nb_initials = 1;
    C->initials = malloc(sizeof(*(C->initials)));
    C->initials[0] = 1;

    // final tab
    C->nb_finals = 1;
    C->finals = malloc(sizeof(*(C->finals)));
    C->finals[0] = A->nb_states + 1;

    return NULL;
}
// End Nathan's function

// Begin Arthur's function

bool is_state_initial(nfa *A, int state){
    bool output = false;
    for (int i = 0; i<A->nb_initials;){
        if (A->initials[i] == state){
        output = true;
        break;
        }
    }
    return output;
}

nfa *nfa_trim(nfa *A)
{   
    nfa *A2 = nfa_copy(A);
    for (int i = 0; i < A2->nb_states; i++){
        if (!is_state_final(A2, i) && !is_state_initial(A2, i)) { // On regarde si l'état considéré n'est ni initial ni final
            bool is_accessible = false;
            for (int j = 0; j < A2->transitions->nb_labels; j++){  
                if (A2->transitions->to_array[j] == i){ // On regarde si l'état n'est accessible pas par un autre état
                    is_accessible = true;
                }
            }
            if (!is_accessible){    // On échange l'état inaccessible avec le dernier  
                for (int k = 0; k < A2->transitions->nb_labels; k++){
                    if (A2->transitions->to_array[k] == A2->nb_states - 1){
                        A2->transitions->to_array[k] = i;
                    }
                }
                A2->state_names[i] = A2->state_names[A2->nb_states - 1];
                free(A2->state_names[A2->nb_states - 1]);
                A2->nb_states--;
                A2->transitions->nb_states--;
            }
        }
    }
    return A2;
}

nfa *nfa_mirror(nfa *A)
    {
        int *size_one_edge_list; // On passe par une liste de multi_edge pour inverser les in et les out facilement
        one_edge *oe = ngraph_to_edge_list(A->transitions, size_one_edge_list);
        for (int i = 0; i < *size_one_edge_list; i++){
            int tmp = oe[i].from;
            oe[i].from = oe[i].to;
            oe[i].to = tmp;
        }
        ngraph *g = edge_list_to_ngraph(oe, *size_one_edge_list, A->nb_states, A->nb_letters);
        free(oe);
        nfa *A2 = nfa_copy(A);
        A2->transitions = g;

        int *tmp = A2->initials;
        A2->initials = A2->finals;
        A2->finals = tmp;
        free(tmp);

        int temp = A2->nb_initials;
        A2->nb_initials = A2->nb_finals;
        A2->nb_finals = temp;
        return A2;
}

nfa *nfa_elimeps(nfa *A) 
{
    int *size_one_edge_list;
    one_edge *oe = ngraph_to_edge_list(A->transitions, size_one_edge_list); // on va passer par une one_edge_list encore une fois, plus facile à manipuler
    int esp_count = 0;

    for (int i = 0; i < *size_one_edge_list; i++){          // On compte les transitions epsilons pour les soustraire à la création de oe2
        if (oe[i].label == A->transitions->nb_labels){
            esp_count++;
        }
    }
    one_edge *oe2 = malloc(sizeof(one_edge) * (*size_one_edge_list - esp_count));
    int j = 0; // Variable d'incrémentation pour oe2
    for (int i = 0 ; i < *size_one_edge_list - esp_count; i ++){
        if (oe[i].label < A->transitions->nb_labels){
            oe2[j] = oe[i];
            j++;            
        }
    }
    free(oe);
    ngraph *g = edge_list_to_ngraph(oe2, *size_one_edge_list, A->nb_states, A->nb_letters);
    free(oe2);
    nfa *A2 = nfa_copy(A);
    free(A2->transitions->to_array);
    free(A2->transitions->intervals);
    free(A2->transitions);
    A2->transitions = g;
    return A2;
}

barray *nfa_compute_runs(nfa *A, const char *s)
{    
    barray * b = create_barray(A->nb_states);
    int *one_edge_list_size;                              // On va utiliser un multi_edge pour faire des & bits à bits
    one_edge *oe = ngraph_to_edge_list(A->transitions, one_edge_list_size);

    for (int i = 0; i< A->nb_initials; i++){             // On initialise le barray qu'on va rendre à la fin de la fonction
        settrue_barray(b, A->initials[i]);
    }

    for (int i=0; i<strlen(s); i++){
        char letter = s[i];
        int index_letter = -1;
        for (int j= 0; j<A->nb_letters; j++){           // On cherche si la lettre fait partie de l'alphabet.
            if (letter == A->letter_names[j]){
                index_letter = j;
            }
        }
        if (index_letter != -1){                        
            barray * new_b = create_barray(A->nb_states);    // On crée un barray par lettre de la chaine que l'on supprime en passant à la prochaine lettre
            for (int j = 0; j < *one_edge_list_size;j++){
                if (getval_barray(b,oe[j].from) && oe[j].label == index_letter){
                    settrue_barray(new_b, oe[j].to);
                }
            }
            b = new_b;
            delete_barray(new_b);
        }
    }

    return b;


}
// End Arthur's function
