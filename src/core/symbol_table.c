//
// Created by botnet64 on 11/8/26.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <core/symbol_table.h>

// region prototypes


// endregion

typedef struct {
    const char *symbol;
    uint16_t binary_code;
} instruction;

// region pseudoinstruction
const char *pseudoinstructions[] = {
    "ORG",
    "END",
    "DEC",
    "HEX"
};

enum pseudoinstruction; // defined in header

enum pseudoinstruction get_pseudoinstruction(const char *token) {
    for (int i = 0; i < 4; i++) {
        if (strcmp(token, pseudoinstructions[i]) == 0) {
            return i;
        }
    }
    return INVALID_pseudoinstruction;
}

/**
 * @param token Input token
 * @return 1 if token is valid pseudoinstruction, 0 otherwise
 */
int is_pseudoinstruction(const char *token) {
    return get_pseudoinstruction(token) != INVALID_pseudoinstruction;
}

// endregion

// region MRI
const instruction memory_instructions[] = {
    {.symbol = "AND", .binary_code = 0x0000},
    {.symbol = "ADD", .binary_code = 0x1000},
    {.symbol = "LDA", .binary_code = 0x2000},
    {.symbol = "STA", .binary_code = 0x3000},
    {.symbol = "BUN", .binary_code = 0x4000},
    {.symbol = "BSA", .binary_code = 0x5000},
    {.symbol = "ISZ", .binary_code = 0x6000}
};

/**
 * Returns the index to use inside memory_instruction to get the respective instruction for the token
 * @param token input token
 * @return The index of the instruction inside the MRI table, -1 if token is invalid MRI
 */
int get_memory_instruction_index(const char *token) {
    for (int i = 0; i < 7; i++) {
        if (strcmp(token, memory_instructions[i].symbol) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Returns whether the given token is a valid MRI or not.
 * @param token Input token
 * @return 1 if true, 0 if false
 */
int is_memory_instruction(const char *token) {
    return get_memory_instruction_index(token) != -1;
}

/**
 * Returns the string of assembly text representing the given token
 * Kinda pointless now that I think about it; return value = token (in value)
 * @param token Input token
 * @return Null if token is not a valid MRI, const char* if valid MRI
 */
const char *get_memory_instruction_symbol(const char *token) {
    if (!is_memory_instruction(token)) {
        return NULL;
    }

    return memory_instructions[get_memory_instruction_index(token)].symbol;
}

/**
 * Returns the binary code representing the given token
 * @param token Input token
 * @param code The binary code representing the given token. Will only be assigned if token is valid MRI
 * @return 1 if token was valid MRI, 0 if token is invalid MRI
 */
int get_memory_instruction_code(const char *token, uint16_t *code) {
    if (!is_memory_instruction(token)) {
        return 0;
    }

    *code = memory_instructions[get_memory_instruction_index(token)].binary_code;
    return 1;
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

/**
 * Returns the index to use inside non_memory_instruction to get the respective instruction for the token
 * @param token input token
 * @return The index of the instruction inside the non-MRI table, -1 if token is invalid non-MRI
 */
int get_non_memory_instruction_index(const char *token) {
    for (int i = 0; i < 18; i++) {
        if (strcmp(token, non_memory_instructions[i].symbol) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Returns whether the given token is a valid non-MRI or not.
 * @param token Input token
 * @return 1 if true, 0 if false
 */
int is_non_memory_instruction(const char *token) {
    return get_non_memory_instruction_index(token) != -1;
}

/**
 * Returns the string of assembly text representing the given token
 * Kinda pointless now that I think about it; return value = token (in value)
 * @param token Input token
 * @return Null if token is not a valid non-MRI, const char* if valid non-MRI
 */
const char *get_non_memory_instruction_symbol(const char *token) {
    if (!is_non_memory_instruction(token)) {
        return NULL;
    }

    return non_memory_instructions[get_non_memory_instruction_index(token)].symbol;
}

/**
 * Returns the binary code representing the given token
 * @param token Input token
 * @param code The binary code representing the given token. Will only be assigned if token is valid MRI
 * @return 1 if token was valid MRI, 0 if token is invalid MRI
 */
int get_non_memory_instruction_code(const char *token, uint16_t *code) {
    if (!is_non_memory_instruction(token)) {
        return 0;
    }

    *code = non_memory_instructions[get_non_memory_instruction_index(token)].binary_code;
    return 1;
}

// endregion

// region label
typedef struct label {
    char symbol[4];
    uint16_t address;
    struct label *next;
} label;

label *label_list = NULL;

/**
 * @param token Input token
 * @return Pointer to the label structure representing the token, NULL if invalid label
 */
label *get_label(const char *token) {
    label *l = label_list;
    while (l != NULL) {
        if (strcmp(token, l->symbol) == 0) {
            return l;
        }
        l = l->next;
    }
    return NULL;
}

/**
 * @param token Input token
 * @return 1 if token is a label in the symbol table, 0 otherwise
 */
int is_label(const char *token) {
    return get_label(token) != NULL;
}

/**
 * Returns the binary address of the given label
 * @param token Input token
 * @param address The binary address of the token if the label exists in the symbol table
 * @return 1 if the label exists in the symbol table, 0 if it does not
 */
int get_label_address(const char *token, uint16_t *address) {
    label *symbol = get_label(token);
    if (symbol == NULL) {
        return 0;
    }

    *address = symbol->address;
    return 1;
}

/**
 * Adds the label with its address to the symbol table
 * @param symbol The label's symbol, must be a valid label
 * @param address The labels memory address
 * @return 0 on success, 1 on duplicate symbol and 2 on memory allocation failure
 */
int add_label(const char *symbol, const uint16_t address) {
    // check for duplicates
    if (get_label(symbol) != NULL) {
        return 1;
    }

    label *new_label = malloc(sizeof(label));
    if (new_label == NULL) {
        return 2;
    }
    // strlen(symbol) <= 3; safe to copy into 4 byte buffer
    strcpy(new_label->symbol, symbol);
    new_label->address = address;
    new_label->next = NULL;

    if (label_list == NULL) {
        label_list = new_label;
    } else {
        new_label->next = label_list;
        label_list = new_label;
    }
    return 0;
}

/**
 * Free the dynamic memory allocated for label names
 */
void free_labels(void) {
    label *current = label_list;

    while (current != NULL) {
        label *next = current->next;
        free(current);
        current = next;
    }

    label_list = NULL;
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
int is_valid_label(char *token) {
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

// endregion

/**
 * @param token Input token
 * @return 1 if the token is a reserved keyword, 0 if it is not
 */
int is_reserved(const char *token) {
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
