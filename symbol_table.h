//
// Created by botnet64 on 11/8/26.
//

#ifndef ASSEMBLER_SYMBOL_TABLE_H
#define ASSEMBLER_SYMBOL_TABLE_H

#include <stdio.h>

#define LINE_LENGTH 128

void first_pass(FILE* input);
void add_label(char* label, int lc);
int is_label(const char token[5]);

#endif //ASSEMBLER_SYMBOL_TABLE_H