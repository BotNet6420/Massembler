//
// Created by botnet64 on 11/10/26.
//

#ifndef ASSEMBLER_ASSEMBLE_H
#define ASSEMBLER_ASSEMBLE_H

/**
 * Generate binary instruction for Mano's basic computer
 * from the given source assembly code
 * @param input null-terminated string of input assembly code to process
 * @param output_length integer pointer to return the size of the output in bytes
 * @return Pointer to the generated code, NULL on errors. Memory must be freed by the caller
 */
unsigned char *assemble(char input[], int *output_length);

#endif //ASSEMBLER_ASSEMBLE_H
