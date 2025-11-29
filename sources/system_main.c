#include "system_main.h"
#include "alloc.h"
#include "nfa.h"
#include "regexp.h"
#include "system_files.h"
#include "system_printing.h"
#include "system_shell_keywords.h"
#include "system_tools.h"
#include <string.h>
#include <time.h>

#include <readline/history.h>
#include <readline/readline.h>

extern const char *all_strings[];

char **keyword_name_completion(const char *, int, int);

char *keyword_name_generator(const char *, int);

char **keyword_name_completion(const char *text, int start, int end) {
    int z                        = start;
    start                        = z;
    z                            = end;
    end                          = z;

    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, keyword_name_generator);
}

char *keyword_name_generator(const char *text, int state) {
    static int index, len;
    const char *name;

    if (!state) {
        index = 0;
        len   = strlen(text);
    }

    while ((name = all_strings[index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc > 0); // pour éviter un warning.

    // Création du répertoire outputs
    system("mkdir -p " OUTPUT_DIR);

    // Readline
    rl_attempted_completion_function = keyword_name_completion;
    // rl_basic_word_break_characters   = " \t\n\"\\'`@$><;|&{(,.=";
    rl_basic_word_break_characters   = " \t.=";

    char *histfile;
    char *str;
    str = strrchr(argv[0], '/');
    if (str != NULL) {
        str++;
    } else {
        str = argv[0];
    }
    CREATE_ZERO_MEMORY(histfile, 10 + strlen(str));
    histfile[0] = '.';
    strcpy(histfile + 1, str);
    strcpy(histfile + 1 + strlen(str), "_history");

    srand(time(NULL));

    using_history();

    int err = read_history(histfile);
    if (err) {
        WARNING("Pas de fichier d'historique trouvé, %d.\n", err);
    }

    keywords_add_all_keys();

    init_objects_array();

    print_title_box(5, true, stdout, 1,
                    "Bienvenue dans LEA (Langages, Expressions et Automates)");

    shell_parse();

    write_history(histfile);

    free(histfile);
    return EXIT_SUCCESS;
}
