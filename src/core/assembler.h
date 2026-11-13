//
// Created by botnet64 on 11/10/26.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

#define ADDRESS_MIN 0
#define ADDRESS_MAX 0xFFF
#include <stdint.h>

int first_pass(int size, char *input[size]);

unsigned char *second_pass(int size, char *input[size], uint16_t *output_size);

unsigned char *assemble(char input[], int *output_length);

#endif //ASSEMBLER_ASSEMBLER_H
