//
// Created by botnet64 on 11/8/26.
//

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

// region prototypes
int is_valid_label(const char token[5]);
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

// region label
typedef struct label {
    char symbol[4];
    uint16_t address;
    struct label * next;
} label;

label * label_list = NULL;

label * get_label(const char token[4]) {
    label * l = label_list;
    while (l != NULL) {
        if (strcmp(token, l->symbol) == 0) {
            return l;
        }
        l = l->next;
    }
    return NULL;
}

int is_label(const char token[4]) {
    return get_label(token) != NULL;
}

/**
 * adds the label with its address to the symbol table
 * @param symbol The label's symbol, at most 3 characters plus a null terminator
 * @param address The labels memory address
 * @return 0 on success, 1 on duplicate symbol and 2 on memory alloation faliure
 */
int add_label(const char symbol[4], uint16_t address) {
    // check for duplicates
    if (get_label(symbol) != NULL) {
        return 1;
    }

    label *new_label = malloc(sizeof(label));
    if (new_label == NULL) {
        return 2;
    }
    strcpy(new_label->symbol, symbol);
    new_label->address = address;
    new_label->next = NULL;

    if (label_list == NULL) {
        label_list = new_label;
    }
    else {
        new_label->next = label_list;
        label_list = new_label;
    }
    return 0;
}

void free_labels(void) {
    label *current = label_list;

    while (current != NULL) {
        label *next = current->next;
        free(current);
        current = next;
    }

    label_list = NULL;
}
// endregion

void first_pass(FILE* input) {
    char line[LINE_LENGTH];
    int line_counter = 0;

    while (fgets(line, LINE_LENGTH, input) != NULL) {

        char label[] = {line[0], line[1], line[2], line[3], '\0'};
        if (is_valid_label(label)) {
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

int is_valid_label(const char token[5]) {
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