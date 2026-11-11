//
// Created by botnet64 on 11/10/26.
//

#include <core/assembler.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <core/symbol_table.h>


/**
 * Perform the first assembler pass on the source code to generate
 * the symbol table for user labels with their addresses
 * @param input the source code file
 * @return 0 on success, non-zero on failure
 */
int first_pass(FILE *input) {
    char line[LINE_LENGTH];
    uint16_t line_counter = 0;
    int file_line = 0; // to be used by error messages to report the problematic line

    while (fgets(line, LINE_LENGTH, input) != NULL) {
        file_line++;
        if (strchr(line, '\n') == NULL && feof(input) == 0) {
            // TODO: ERROR(line too long; cannot parse)
            return 1;
        }

        char copy[LINE_LENGTH];
        strcpy(copy, line);

        char *part1 = strtok(copy, " \t\r\n");
        char *part2 = strtok(NULL, " \t\r\n");

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
            if (sscanf(part2, " %x %c", &number, &dummy) != 1) {
                // TODO: ERROR(invalid address)
                return 1;
            }

            // catch invalid extra token
            if (strtok(NULL, " \t\r\n") != NULL) {
                // TODO: ERROR(invalid token)
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

int translate_MRI(const char *instruction, int file_line, uint16_t *mem_cell) {
    char copy[LINE_LENGTH];

    if (instruction == NULL) {
        // TODO: FATAL(NULL pointer dereference (MRI instruction))
        return 1;
    }
    if (mem_cell == NULL) {
        // TODO: FATAL(NULL pointer dereference (MRI mem_cell))
        return 1;
    }

    strcpy(copy, instruction);

    const char *opcode = strtok(copy, " \t\r\n");
    const char *address = strtok(NULL, " \t\r\n");
    const char *indirection = strtok(NULL, " \t\r\n"); // [optional] I
    const char *token4 = strtok(NULL, " \t\r\n"); // Must be NULL

    uint16_t opcode_binary_code = 0;
    uint16_t address_binary_code = 0;
    uint16_t indirection_binary_code = 0;

    // Opcode translation
    if (opcode == NULL) {
        // TODO: ERROR(missing MRI)
        return 1;
    }

    if (!is_memory_instruction(opcode)) {
        // TODO: ERROR(invalid MRI)
        return 1;
    }

    if (get_memory_instruction_code(opcode, &opcode_binary_code) != 1) {
        // TODO: ERROR(instruction not found)
        return 1;
    }


    // Address translation
    if (address == NULL) {
        // TODO: ERROR(missing address)
        return 1;
    }

    if (get_label_address(address, &address_binary_code) != 1) {
        // TODO: ERROR(invalid label)
        return 1;
    }


    // Check indirection
    if (indirection != NULL) {
        if (strcmp(indirection, "I") == 0) {
            indirection_binary_code = 0x8000;
        } else {
            // TODO: ERROR(invalid token, expected I)
            return 1;
        }
    }


    if (token4 != NULL) {
        // TODO: ERROR(invalid token)
        return 1;
    }


    *mem_cell = opcode_binary_code | address_binary_code | indirection_binary_code;
    return 0;
}

int translate_non_MRI(const char *instruction, int file_line, uint16_t *mem_cell) {
    char copy[LINE_LENGTH];

    if (instruction == NULL) {
        // TODO: FATAL(NULL pointer dereference (non-MRI instruction))
        return 1;
    }
    if (mem_cell == NULL) {
        // TODO: FATAL(NULL pointer dereference (non-MRI mem_cell))
        return 1;
    }

    strcpy(copy, instruction);

    const char *opcode = strtok(copy, " \t\r\n");
    const char *token2 = strtok(NULL, " \t\r\n"); // Must be NULL

    uint16_t opcode_binary_code = 0;

    // Opcode translation
    if (opcode == NULL) {
        // TODO: ERROR(missing non-MRI)
        return 1;
    }

    if (!is_non_memory_instruction(opcode)) {
        // TODO: ERROR(invalid non-MRI)
        return 1;
    }

    if (get_non_memory_instruction_code(opcode, &opcode_binary_code) != 1) {
        // TODO: ERROR(instruction not found)
        return 1;
    }


    if (token2 != NULL) {
        // TODO: ERROR(invalid token)
        return 1;
    }

    *mem_cell = opcode_binary_code;
    return 0;
}

int second_pass(FILE *input) {
    char line[LINE_LENGTH];
    uint16_t output_buffer[4096] = {0};

    uint16_t line_counter = 0;
    int file_line = 0;

    while (fgets(line, LINE_LENGTH, input) != NULL) {
        file_line++;

        if (strchr(line, '\n') == NULL && feof(input) == 0) {
            // TODO: ERROR(line too long; cannot parse)
            return 1;
        }

        char copy[LINE_LENGTH];
        strcpy(copy, line);
        const char *instruction = line;

        char *token = strtok(copy, " \t\r\n");

        if (token == NULL) {
            continue;
        }

        // skip label token
        if (token[strlen(token) - 1] == ',') {
            token = strtok(NULL, " \t\r\n");

            // Cannot have empty label line
            if (token == NULL) {
                // TODO: ERROR(missing instruction)
                return 1;
            }

            // ORG and END cannot have labels
            if (strcmp(token, "ORG") == 0) {
                // TODO: Error(ORG cannot have label)
                return 1;
            }
            if (strcmp(token, "END") == 0) {
                // TODO: Error(END cannot have label)
                return 1;
            }

            // advance line pointer to start of opcode
            instruction = strchr(line, ',') + 1;
            while (isspace((unsigned char)*instruction)) {
                instruction++;
            }
        }

        // MRI
        if (is_memory_instruction(token)) {
            // address overflow
            if (line_counter > 0xfff) {
                // TODO: ERROR(address overflow; program too large)
                return 1;
            }

            int s = translate_MRI(instruction, file_line, output_buffer + line_counter);

            if (s != 0) {
                return 1;
            }

            line_counter++;
            continue;
        }

        // non-MRI
        if (is_non_memory_instruction(token)) {
            // address overflow
            if (line_counter > 0xfff) {
                // TODO: ERROR(address overflow; program too large)
                return 1;
            }

            int s = translate_non_MRI(instruction, file_line, output_buffer + line_counter);

            if (s != 0) {
                return 1;
            }

            line_counter++;
            continue;
        }

        // pseudoinstruction
        if (is_pseudoinstruction(token)) {
            char *token2 = strtok(NULL, " \t\r\n");
            char *token3 = strtok(NULL, " \t\r\n");

            // self address checking
            switch (get_pseudoinstruction(token)) {
                case ORG: {
                    // check address exists
                    if (token2 == NULL) {
                        // TODO: ERROR(ORG missing operand)
                        return 1;
                    }

                    // try parse the number
                    unsigned int number = 0;
                    if (sscanf(token2, " %x", &number) != 1) {
                        // TODO: ERROR(no address found)
                        return 1;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    if (sscanf(token2, " %x %c", &number, &dummy) != 1) {
                        // TODO: ERROR(invalid address)
                        return 1;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        // TODO: ERROR(invalid token)
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

                case END: {
                    if (token2 != NULL) {
                        // TODO: ERROR(unexpected operand after END)
                        return 1;
                    }

                    FILE *a = fopen("a.txt", "w");
                    for (int i = 0; i < line_counter; i++) {
                        fprintf(a, "%4d (%3x): %x\n", i, i, output_buffer[i]);
                    }
                    fclose(a);

                    // output code
                    FILE *output = fopen("a.out", "wb");
                    fwrite(output_buffer, sizeof(uint16_t), line_counter, output);
                    fclose(output);

                    return 0;
                }

                default: {
                    break;
                }
            }


            // address overflow
            if (line_counter > 0xfff) {
                // TODO: ERROR(address overflow; program too large)
                return 1;
            }
            switch (get_pseudoinstruction(token)) {
                case DEC: {
                    if (token2 == NULL) {
                        // TODO: ERROR(missing operand)
                        return 1;
                    }

                    // try parse the number
                    int number = 0;
                    if (sscanf(token2, " %d", &number) != 1) {
                        // TODO: ERROR(no number found)
                        return 1;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    if (sscanf(token2, " %d %c", &number, &dummy) != 1) {
                        // TODO: ERROR(invalid address)
                        return 1;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        // TODO: ERROR(invalid token)
                        return 1;
                    }

                    if (number > INT16_MAX) {
                        // TODO: ERROR(literal too big)
                        return 1;
                    }

                    if (number < INT16_MIN) {
                        // TODO: ERROR(literal too small)
                        return 1;
                    }

                    output_buffer[line_counter] = (uint16_t) number;
                    line_counter++;
                    continue;
                }

                case HEX: {
                    if (token2 == NULL) {
                        // TODO: ERROR(missing operand)
                        return 1;
                    }

                    // try parse the number
                    unsigned int number = 0;
                    if (sscanf(token2, " %x", &number) != 1) {
                        // TODO: ERROR(no number found)
                        return 1;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    if (sscanf(token2, " %x %c", &number, &dummy) != 1) {
                        // TODO: ERROR(invalid hex)
                        return 1;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        // TODO: ERROR(invalid token)
                        return 1;
                    }

                    if (number > UINT16_MAX) {
                        // TODO: ERROR(literal too big)
                        return 1;
                    }

                    output_buffer[line_counter] = (uint16_t) number;
                    line_counter++;
                    continue;
                }

                case INVALID_pseudoinstruction: {
                    // TODO: ERROR(invalid instruction)
                    return 1;
                }

                default: {
                    // TODO FATAL(should not have reached here)
                    return 1;
                }
            }
        }

        // TODO: ERROR(invalid instruction)
        return 1;
    }
    // TODO: ERROR(EOF without END instruction)
    return 1;
}
