//
// Created by botnet64 on 11/10/26.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

#include <stdio.h>

#define LINE_LENGTH 128
#define ADDRESS_MIN 0
#define ADDRESS_MAX

int first_pass(FILE *input);
int second_pass(FILE *input);

#endif //ASSEMBLER_ASSEMBLER_H
