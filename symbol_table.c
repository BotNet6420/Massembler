//
// Created by botnet64 on 11/8/26.
//

#include <string.h>
#include "symbol_table.h"

#include <ctype.h>

void first_pass(FILE* input) {
    char line[LINE_LENGTH];
    int line_counter = 0;

    while (fgets(line, LINE_LENGTH, input) != NULL) {

        char label[] = {line[0], line[1], line[2], line[3], '\0'};
        if (is_label(label)) {
            label[4] = '\0';
            add_label(label, line_counter);
            line_counter++;
            continue;
        }

        label[4] = '\0';
        if (strcmp(label, "ORG") == 0) {
            char* argument = &line[3];
            int new_line_counter = 0;
            if (sscanf(argument, "%d", &new_line_counter) != 1) {
                // TODO: error(expected address)
                return;
            }
            line_counter = new_line_counter;
            continue;
        }

        if (strcmp(label, "END") == 0) {
            return;
        }
        line_counter++;
    }
}

void add_label(char* label, int lc) {
    // TODO: implement symbol label
}

int is_label(const char token[5]) {
    if (!isalpha(token[0]))
        return 0;

    if (token[1] == ',')
        return 1;

    if (!isalnum(token[1]))
        return 0;

    if (token[2] == ',')
        return 1;

    if (!isalnum(token[2]))
        return 0;

    return token[3] == ',';
}