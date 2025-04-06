#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"
#include "utils.h"
#include "parser.h"
#include "ad.h"
int main() {

    const char *input = loadFile("tests/testlex.c");

    Token *tokens = tokenize(input);

    FILE *out = fopen("tests/lista-de-atomi.txt", "w");
    if (!out) {
        perror("Unable to open output file");
        exit(EXIT_FAILURE);
    }

    showTokens(tokens, out);
    parse(tokens);

    fclose(out);
    printf("Tokens written to tests/lista-de-atomi.txt\n");

    return 0;
}
