//
// Created by botnet64 on 11/8/26.
//

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

// region prototypes
int is_valid_label(char* token);
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

/**
 * Perform the first assembler pass on the source code to generate
 * the symbol table for user labels with their addresses
 * @param input the source code file
 * @return 0 on success, non-zero on failure
 */
int first_pass(FILE* input) {
    char line[LINE_LENGTH];
    uint16_t line_counter = 0;
    uint16_t file_counter = 0; // to be used by error messages to report the problematic line

    while (fgets(line, LINE_LENGTH, input) != NULL) {
        file_counter++;
        if (strchr(line, '\n') == NULL && feof(input) == 0) {
            // TODO: ERROR(line too long; cannot parse)
            return 1;
        }

        char copy[LINE_LENGTH];
        strcpy(copy, line);

        char* part1 = strtok(copy, " \t\r\n");
        char* part2 = strtok(NULL, " \t\r\n");

        // skip empty lines
        if (part1 == NULL) {
            continue;
        }

        // check for first column being a label
        if (part1[strlen(part1) - 1] == ',') {
            // address overflow
            if (line_counter > 0xfff) {
                // TODO: ERROR(address overflow; program too large)
                return 1;
            }

            // invalid label format
            if (!is_valid_label(part1)) {
                // TODO: ERROR(invalid label)
                return 1;
            }

            // catch label on empty line
            if (part2 == NULL) {
                // TODO: ERROR(missing instruction)
                return 1;
            }

            // ORG and END cannot have labels
            if (strcmp(part2, "ORG") == 0) {
                // TODO: Error(ORG cannot have label)
                return 1;
            }
            if (strcmp(part2, "END") == 0) {
                // TODO: Error(END cannot have label)
                return 1;
            }

            // try to add the label
            int result = add_label(part1, line_counter);
            if (result == 1) {
                // TODO: ERROR(duplicate symbol)
                return 1;
            }
            if (result == 2) {
                // TODO: ERROR(out-of-memory)
                return 1;
            }
            line_counter++;
            continue;
        }

        // not a label, check ORG
        if (strcmp(part1, "ORG") == 0) {
            // check address exists
            if (part2 == NULL) {
                // TODO: ERROR(ORG missing operand)
                return 1;
            }

            // try parse the number
            unsigned int number = 0;
            if (sscanf(part2, " %x", &number) != 1) {
                // TODO: ERROR(no address found)
                return 1;
            }

            // catch syntax errors (trailing non-whitespace character)
            char dummy;
            if (sscanf(part2, " %x %c", &number, &dummy) != 1 || strtok(NULL, " \t\r\n") != NULL) {
                // TODO: ERROR(invalid address)
                return 1;
            }

            // address out of bounds
            if (number > 0xfff) {
                // TODO: ERROR(address too large)
                return 1;
            }
            line_counter = (uint16_t) number;
            continue;
        }

        // exit first pass
        if (strcmp(part1, "END") == 0) {
            if (part2 != NULL) {
                // TODO: ERROR(unexpected operand after END)
                return 1;
            }

            return 0;
        }

        // address overflow
        if (line_counter > 0xfff) {
            // TODO: ERROR(address overflow; program too large)
            return 1;
        }
        line_counter++;
    }
    // TODO: ERROR(EOF without END instruction)
    return 1;
}

/**
 * @param token a 3character token from the sourcecode
 * @return 1 if the token is a reserved instruction, 0 if it is not
 */
int is_reserved(const char token[4]) {
    if (get_pseudoinstruction(token) != INVALID_pseudoinstruction) {
        return 1;
    }
    if (get_memory_instruction_index(token) != -1) {
        return 1;
    }
    if (get_non_memory_instruction_index(token) != -1) {
        return 1;
    }
    return 0;
}

/**
 * checks a string for label validity
 * 1. Must start with a letter
 * 2. must be at least 1, and at most 3 character
 * 3. characters 2 and 3 can be alphanumeric
 * 4. must be terminated by a comma
 * @param token the string to be tested for label validity (must contain the trailing comma)
 * @return 1 if the string is a valid label, 0 if not
 */
int is_valid_label(char* token) {
    if (token == NULL)
        return 0;

    size_t length = strlen(token);
    if (length <= 0) {
        return 0;
    }

    if (token[length - 1] != ',') {
        return 0;
    }

    token[length - 1] = '\0';
    length -= 1;

    if (length < 1 || length > 3)
        return 0;

    if (!isalpha((unsigned char)token[0]))
        return 0;

    for (size_t i = 1; i < length; i++) {
        if (!isalnum((unsigned char)token[i]))
            return 0;
    }

    return !is_reserved(token);
}

void dump_labels() {
    label *current = label_list;
    while (current != NULL) {
        printf("%3s - %X\n", current->symbol, current->address);
        current = current->next;
    }
}