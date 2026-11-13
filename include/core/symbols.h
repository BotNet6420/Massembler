//
// Created by botnet64 on 11/10/26.
//

#ifndef ASSEMBLER_SYMBOLS_H
#define ASSEMBLER_SYMBOLS_H

int is_pseudoinstruction(const char *token);

int is_memory_instruction(const char *token);

int is_non_memory_instruction(const char *token);

int is_label(const char *token);

int is_valid_label(char *token);

int is_reserved(const char *token);

#endif //ASSEMBLER_SYMBOLS_H
