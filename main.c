#include <stdio.h>
#include <string.h>

extern FILE *yyin, *output;

extern int yyparse();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
        return 0;
    }
    yyin = fopen(argv[1], "r");

    char *ext = strrchr(argv[1], '.');
    *(ext + 1) = 's';
    output = fopen(argv[1], "w");

    yyparse();

    fclose(yyin);
    return 0;
}

