//
// Created by botnet64 on 11/8/26.
//

#ifndef ASSEMBLER_SYMBOL_TABLE_H
#define ASSEMBLER_SYMBOL_TABLE_H

#include <stdint.h>

enum pseudoinstruction {
    INVALID_pseudoinstruction = -1,
    ORG = 0,
    END = 1,
    DEC = 2,
    HEX = 3,
};


enum pseudoinstruction get_pseudoinstruction(const char *token);

int is_pseudoinstruction(const char *token);

int is_memory_instruction(const char *token);

const char *get_memory_instruction_symbol(const char *token);

int get_memory_instruction_code(const char *token, uint16_t *code);

int is_non_memory_instruction(const char *token);

const char *get_non_memory_instruction_symbol(const char *token);

int get_non_memory_instruction_code(const char *token, uint16_t *code);

int is_label(const char *token);

int get_label_address(const char *token, uint16_t *address);

int add_label(const char *symbol, uint16_t address);

void free_labels(void);

int is_valid_label(char *token);

void dump_labels();

int is_reserved(const char *token);

#endif //ASSEMBLER_SYMBOL_TABLE_H
