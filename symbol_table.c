//
// Created by botnet64 on 11/8/26.
//

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include "symbol_table.h"

// region prototypes
void add_label(char* label, int lc);
int is_label(const char token[5]);
// endregion

typedef struct {
    const char* symbol;
    uint16_t binary_code;
} instruction;

// region pseudoinstruction
const char *pseudoinstruction_table[] = {
    "ORG",
    "END",
    "DEC",
    "HEX"
};

enum pseudoinstruction {
    INVALID_pseudoinstruction = -1,
    ORG = 0,
    END = 1,
    DEC = 2,
    HEX = 3,
};


enum pseudoinstruction get_pseudoinstruction(const char token[4]) {
    for (int i = 0; i < 4; i++) {
        if (strcmp(token, pseudoinstruction_table[i]) == 0) {
            return i;
        }
    }
    return INVALID_pseudoinstruction;
}

int is_pseudoinstruction(const char token[4]) {
    return get_pseudoinstruction(token) != INVALID_pseudoinstruction;
}
// endregion

// region MRI
const instruction memory_instruction[] = {
    {.symbol = "AND", .binary_code = 0x0000},
    {.symbol = "ADD", .binary_code = 0x1000},
    {.symbol = "LDA", .binary_code = 0x2000},
    {.symbol = "STA", .binary_code = 0x3000},
    {.symbol = "BUN", .binary_code = 0x4000},
    {.symbol = "BSA", .binary_code = 0x5000},
    {.symbol = "ISZ", .binary_code = 0x6000}
};

int get_memory_instruction_index(const char token[4]) {
    for (int i = 0; i < 7; i++) {
        if (strcmp(token, memory_instruction[i].symbol) == 0) {
            return i;
        }
    }
    return -1;
}

int is_memory_instruction(const char token[4]) {
    return get_memory_instruction_index(token) != -1;
}
// endregion

// region non-MRI
const instruction non_memory_instructions[] = {
    {.symbol = "CLA", .binary_code = 0x7800}, // register reference instructions
    {.symbol = "CLE", .binary_code = 0x7400},
    {.symbol = "CMA", .binary_code = 0x7200},
    {.symbol = "CME", .binary_code = 0x7100},
    {.symbol = "CIR", .binary_code = 0x7080},
    {.symbol = "CIL", .binary_code = 0x7040},
    {.symbol = "INC", .binary_code = 0x7020},
    {.symbol = "SPA", .binary_code = 0x7008},
    {.symbol = "SNA", .binary_code = 0x7004},
    {.symbol = "SZA", .binary_code = 0x7004},
    {.symbol = "SZE", .binary_code = 0x7002},
    {.symbol = "HLT", .binary_code = 0x7001},
    {.symbol = "INP", .binary_code = 0xF800}, // I/O reference instruction
    {.symbol = "OUT", .binary_code = 0xF400},
    {.symbol = "SKI", .binary_code = 0xF200},
    {.symbol = "SKO", .binary_code = 0xF100},
    {.symbol = "ION", .binary_code = 0xF080},
    {.symbol = "IOF", .binary_code = 0xF040}
};

int get_non_memory_instruction_index(const char token[4]) {
    for (int i = 0; i < 18; i++) {
        if (strcmp(token, non_memory_instructions[i].symbol) == 0) {
            return i;
        }
    }
    return -1;
}

int is_non_memory_instruction(const char token [4]) {
    return get_memory_instruction_index(token) != -1;
}
// endregion

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