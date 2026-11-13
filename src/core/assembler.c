//
// Created by botnet64 on 11/10/26.
//

#include <core/assembler.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <core/symbol_table.h>

#include "logger.h"

typedef struct linked_char_list {
    char *vale;
    struct linked_char_list *next;
} linked_char_list;

void write_big_endian(uint16_t number, unsigned char *output);

/**
 * Perform the first assembler pass on the source code to generate
 * the symbol table for user labels with their addresses
 * @param size Amount of input lines
 * @param input char* array, each element being a null-terminated comment-striped line of input
 * @return 0 on success, non-zero on failure
 */
int first_pass(const int size, char *input[size]) {
    caller_details caller = {
        .section_name = "assembler",
        .subsection_name = "first_pass",
    };
    error_details error = {};

    if (input == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "NULL input";
        log_stderr(caller, error, FATAL);
        return 1;
    }

    const char *line = input[0];
    uint16_t line_counter = 0;
    int file_line = 0;

    for (int i = 0; i < size; i++) {
        file_line++;
        if (input[i] == NULL) {
            caller.line_number = __LINE__ - 1;
            error.message = "NULL pointer for input line";
            error.line_number = file_line;
            log_stderr(caller, error, FATAL);
            return 1;
        }

        line = input[i];
        char copy[strlen(line) + 1];
        strcpy(copy, line);
        error.line_number = file_line;
        error.text = line;

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
                caller.line_number = __LINE__ - 1;
                char m[128] = "Instruction address too big";
                char n[128];
                sprintf(n, " (%04x)", line_counter);
                strcat(m, n);
                error.message = m;
                error.character_number = 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // invalid label format
            if (!is_valid_label(part1)) {
                caller.line_number = __LINE__ - 1;
                error.message = "Invalid label format";
                error.character_number = strstr(line, part1) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // catch label on empty line
            if (part2 == NULL) {
                caller.line_number = __LINE__ - 1;
                error.message = "Labeled line missing instruction";
                error.character_number = strstr(line, part1) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // ORG and END cannot have labels
            if (strcmp(part2, "ORG") == 0) {
                caller.line_number = __LINE__ - 1;
                error.message = "ORG instruction cannot have a label";
                error.character_number = strstr(line, part1) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }
            if (strcmp(part2, "END") == 0) {
                caller.line_number = __LINE__ - 1;
                error.message = "END instruction cannot have a label";
                error.character_number = strstr(line, part1) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // try to add the label
            int result = add_label(part1, line_counter);
            if (result == 1) {
                caller.line_number = __LINE__ - 1;
                error.message = "Duplicated label";
                error.character_number = strstr(line, part1) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }
            if (result == 2) {
                caller.line_number = __LINE__ - 1;
                error.message = "Out of memory";
                return 1;
            }
            line_counter++;
            continue;
        }

        // not a label, check ORG
        if (strcmp(part1, "ORG") == 0) {
            // check address exists
            if (part2 == NULL) {
                caller.line_number = __LINE__ - 1;
                error.message = "ORG instruction missing address operand";
                error.character_number = strstr(line, part1) - line + 5;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // try parse the number
            unsigned int number = 0;
            if (sscanf(part2, " %x", &number) != 1) {
                caller.line_number = __LINE__ - 1;
                error.message = "Could not parse HEX address";
                error.character_number = strstr(line, part2) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // catch syntax errors (trailing non-whitespace character)
            char dummy;
            int n = 0;
            if (sscanf(part2, " %x %n%c", &number, &n, &dummy) != 1) {
                caller.line_number = __LINE__ - 1;
                error.message = "Could not parse HEX address";
                error.character_number = strstr(line, part2) - line + n + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // catch invalid extra token
            char *part3 = strtok(NULL, " \t\r\n");
            if (part3 != NULL) {
                caller.line_number = __LINE__ - 1;
                error.message = "Expected newline";
                error.character_number = strstr(line, part3) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            // address out of bounds
            if (number > 0xfff) {
                caller.line_number = __LINE__ - 1;
                error.message = "ORG address too big";
                error.character_number = strstr(line, part2) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }
            line_counter = (uint16_t) number;
            continue;
        }

        // exit first pass
        if (strcmp(part1, "END") == 0) {
            if (part2 != NULL) {
                caller.line_number = __LINE__ - 1;
                error.message = "Expected newline";
                error.character_number = strstr(line, part2) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }

            return 0;
        }

        // address overflow
        if (line_counter > 0xfff) {
            caller.line_number = __LINE__ - 1;
            char m[128] = "Instruction address too big";
            char n[128];
            sprintf(n, " (%04x)", line_counter);
            strcat(m, n);
            error.message = m;
            error.character_number = strstr(line, part1) - line + 1;
            log_stderr(caller, error, ERROR);
            return 1;
        }
        line_counter++;
    }
    caller.line_number = __LINE__ - 1;
    error.message = "Reached EOF but no END instruction";
    error.line_number = file_line;
    error.character_number = strlen(line);
    log_stderr(caller, error, ERROR);
    return 1;
}

int translate_MRI(const char *instruction, int file_line, uint16_t *mem_cell) {
    caller_details caller = {
        .section_name = "assembler",
        .subsection_name = "translate_MRI"
    };
    error_details error = {
        .text = instruction,
        .line_number = file_line,
    };

    char buffer[LINE_LENGTH];

    if (instruction == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "No instruction line; This should not happen";
        log_stderr(caller, error, FATAL);
        return 1;
    }
    if (mem_cell == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "No memory cell; This should not happen";
        log_stderr(caller, error, FATAL);
        return 1;
    }

    strcpy(buffer, instruction);
    // advance line pointer to start of opcode
    char *copy = strchr(buffer, ',');
    if (copy == NULL) {
        copy = buffer;
    }
    else {
        copy += 1;
    }
    while (isspace((unsigned char)*copy)) {
        copy++;
    }

    const char *opcode = strtok(copy, " \t\r\n");
    const char *address = strtok(NULL, " \t\r\n");
    const char *indirection = strtok(NULL, " \t\r\n"); // [optional] I
    const char *token4 = strtok(NULL, " \t\r\n"); // Must be NULL

    uint16_t opcode_binary_code = 0;
    uint16_t address_binary_code = 0;
    uint16_t indirection_binary_code = 0;

    // Opcode translation
    if (opcode == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "No instruction in instruction line; This should not happen";
        log_stderr(caller, error, ERROR);
        return 1;
    }

    if (!is_memory_instruction(opcode)) {
        caller.line_number = __LINE__ - 1;
        error.message = "Invalid MRI";
        error.character_number = strstr(instruction, opcode) - instruction + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }

    if (get_memory_instruction_code(opcode, &opcode_binary_code) != 1) {
        caller.line_number = __LINE__ - 1;
        error.message = "MRI not found in symbol table";
        error.character_number = strstr(instruction, opcode) - instruction + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }


    // Address translation
    if (address == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "MRI missing address";
        error.character_number = strstr(instruction, opcode) - instruction + strlen(opcode) + 2;
        log_stderr(caller, error, ERROR);
        return 1;
    }

    if (get_label_address(address, &address_binary_code) != 1) {
        caller.line_number = __LINE__ - 1;
        error.message = "Undefined label";
        error.character_number = strstr(instruction, address) - instruction + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }


    // Check indirection
    if (indirection != NULL) {
        if (strcmp(indirection, "I") == 0) {
            indirection_binary_code = 0x8000;
        } else {
            caller.line_number = __LINE__ - 1;
            error.message = "Expected I or newline";
            error.character_number = strstr(instruction, indirection) - instruction + 1;
            log_stderr(caller, error, ERROR);
            return 1;
        }
    }


    if (token4 != NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "Expected newline";
        error.character_number = strstr(instruction, token4) - instruction + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }


    uint16_t binary = opcode_binary_code | address_binary_code | indirection_binary_code;
    write_big_endian(binary, (unsigned char *) mem_cell);
    return 0;
}

int translate_non_MRI(const char *instruction, int file_line, uint16_t *mem_cell) {
    caller_details caller = {
        .section_name = "assembler",
        .subsection_name = "translate_non_MRI"
    };
    error_details error = {
        .text = instruction,
        .line_number = file_line,
    };

    char buffer[LINE_LENGTH];

    if (instruction == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "No instruction line; This should not happen";
        log_stderr(caller, error, FATAL);
        return 1;
    }
    if (mem_cell == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "No memory cell; This should not happen";
        log_stderr(caller, error, FATAL);
        return 1;
    }

    strcpy(buffer, instruction);
    // advance line pointer to start of opcode
    char *copy = strchr(buffer, ',');
    if (copy == NULL) {
        copy = buffer;
    }
    else {
        copy += 1;
    }
    while (isspace((unsigned char)*copy)) {
        copy++;
    }

    const char *opcode = strtok(copy, " \t\r\n");
    const char *token2 = strtok(NULL, " \t\r\n"); // Must be NULL

    uint16_t opcode_binary_code = 0;

    // Opcode translation
    if (opcode == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "No instruction in instruction line; This should not happen";
        log_stderr(caller, error, ERROR);
        return 1;
    }

    if (!is_non_memory_instruction(opcode)) {
        caller.line_number = __LINE__ - 1;
        error.message = "Invalid non-MRI";
        error.character_number = strstr(instruction, opcode) - instruction + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }

    if (get_non_memory_instruction_code(opcode, &opcode_binary_code) != 1) {
        caller.line_number = __LINE__ - 1;
        error.message = "non-MRI not found in symbol table";
        error.character_number = strstr(instruction, opcode) - instruction + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }


    if (token2 != NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "Expected newline";
        error.character_number = strstr(instruction, token2) - instruction + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }

    uint16_t binary = opcode_binary_code;
    write_big_endian(binary, (unsigned char *) mem_cell);
    return 0;
}

unsigned char *second_pass(const int size, char *input[size], uint16_t *output_size) {
    caller_details caller = {
        .section_name = "assembler",
        .subsection_name = "second_pass",
    };
    error_details error = {};

    if (input == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "NULL input";
        log_stderr(caller, error, FATAL);
        return NULL;
    }


    uint16_t *output_buffer = malloc(sizeof(uint16_t) * 4096);
    if (output_buffer == NULL) {
        caller.line_number = __LINE__ - 1;
        error.message = "Out of memory";
        log_stderr(caller, error, FATAL);
        return NULL;
    }
    for (int i = 0; i < 4096; i++) {
        output_buffer[i] = 0;
    }

    const char *line;
    uint16_t line_counter = 0;
    int file_line = 0;

    for (int i = 0; i < size; i++) {
        file_line++;
        if (input[i] == NULL) {
            caller.line_number = __LINE__ - 1;
            error.message = "NULL pointer for input line";
            error.line_number = file_line;
            log_stderr(caller, error, FATAL);
            free(output_buffer);
            return NULL;
        }

        line = input[i];
        char copy[strlen(line) + 1];
        strcpy(copy, line);
        const char *instruction = line;
        error.line_number = file_line;
        error.text = line;

        char *token = strtok(copy, " \t\r\n");

        if (token == NULL) {
            continue;
        }

        // skip label token
        if (token[strlen(token) - 1] == ',') {
            char *label = token;
            token = strtok(NULL, " \t\r\n");

            // Cannot have empty label line
            if (token == NULL) {
                caller.line_number = __LINE__ - 1;
                error.message = "Labeled line missing instruction";
                error.character_number = strstr(line, label) - line + 1;
                log_stderr(caller, error, ERROR);
                free(output_buffer);
                return NULL;
            }

            // ORG and END cannot have labels
            if (strcmp(token, "ORG") == 0) {
                caller.line_number = __LINE__ - 1;
                error.message = "ORG instruction cannot have a label";
                error.character_number = strstr(line, label) - line + 1;
                log_stderr(caller, error, ERROR);
                free(output_buffer);
                return NULL;
            }
            if (strcmp(token, "END") == 0) {
                caller.line_number = __LINE__ - 1;
                error.message = "END instruction cannot have a label";
                error.character_number = strstr(line, label) - line + 1;
                log_stderr(caller, error, ERROR);
                free(output_buffer);
                return NULL;
            }
        }

        // MRI
        if (is_memory_instruction(token)) {
            // address overflow
            if (line_counter > 0xfff) {
                caller.line_number = __LINE__ - 1;
                char m[128] = "Instruction address too big";
                char n[128];
                sprintf(n, " (%04x)", line_counter);
                strcat(m, n);
                error.message = m;
                error.character_number = strstr(line, token) - line + 1;
                log_stderr(caller, error, ERROR);
                free(output_buffer);
                return NULL;
            }

            int s = translate_MRI(instruction, file_line, output_buffer + line_counter);

            if (s != 0) {
                free(output_buffer);
                return NULL;
            }

            line_counter++;
            continue;
        }

        // non-MRI
        if (is_non_memory_instruction(token)) {
            // address overflow
            if (line_counter > 0xfff) {
                caller.line_number = __LINE__ - 1;
                char m[128] = "Instruction address too big";
                char n[128];
                sprintf(n, " (%04x)", line_counter);
                strcat(m, n);
                error.message = m;
                error.character_number = strstr(line, token) - line + 1;
                log_stderr(caller, error, ERROR);
                free(output_buffer);
                return NULL;
            }

            int s = translate_non_MRI(instruction, file_line, output_buffer + line_counter);

            if (s != 0) {
                free(output_buffer);
                return NULL;
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
                        caller.line_number = __LINE__ - 1;
                        error.message = "ORG instruction missing address operand";
                        error.character_number = strstr(line, token) - line + 5;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // try parse the number
                    unsigned int number = 0;
                    if (sscanf(token2, " %x", &number) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse HEX address";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    int n = 0;
                    if (sscanf(token2, " %x %n%c", &number, &n, &dummy) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse HEX address";
                        error.character_number = strstr(line, token2) - line + n + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Expected newline";
                        error.character_number = strstr(line, token3) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // address out of bounds
                    if (number > 0xfff) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "ORG address too big";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }
                    line_counter = (uint16_t) number;
                    continue;
                }

                case END: {
                    if (token2 != NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Expected newline";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    *output_size = line_counter * (sizeof(uint16_t) / sizeof(unsigned char));
                    return (unsigned char *) output_buffer;
                }

                default: {
                    break;
                }
            }


            // address overflow
            if (line_counter > 0xfff) {
                caller.line_number = __LINE__ - 1;
                char m[128] = "Instruction address too big";
                char n[128];
                sprintf(n, " (%04x)", line_counter);
                strcat(m, n);
                error.message = m;
                error.character_number = strstr(line, token) - line + 1;
                log_stderr(caller, error, ERROR);
                free(output_buffer);
                return NULL;
            }
            switch (get_pseudoinstruction(token)) {
                case DEC: {
                    if (token2 == NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "DEC instruction missing operand";
                        error.character_number = strstr(line, token) - line + 5;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // try parse the number
                    int number = 0;
                    if (sscanf(token2, " %d", &number) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse decimal number";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    int n = 0;
                    if (sscanf(token2, " %d %n%c", &number, &n, &dummy) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse decimal number";
                        error.character_number = strstr(line, token2) - line + n + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Expected newline";
                        error.character_number = strstr(line, token3) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    if (number > INT16_MAX) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "decimal number too big";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    if (number < INT16_MIN) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "decimal number too small";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    write_big_endian((uint16_t) number, (unsigned char*) &output_buffer[line_counter]);
                    line_counter++;
                    continue;
                }

                case HEX: {
                    if (token2 == NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "HEX instruction missing operand";
                        error.character_number = strstr(line, token) - line + 5;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // try parse the number
                    unsigned int number = 0;
                    if (sscanf(token2, " %x", &number) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse hexadecimal number";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    int n = 0;
                    if (sscanf(token2, " %x %n%c", &number, &n, &dummy) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse hexadecimal number";
                        error.character_number = strstr(line, token2) - line + n + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Expected newline";
                        error.character_number = strstr(line, token3) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    if (number > UINT16_MAX) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "hexadecimal number too big";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        free(output_buffer);
                        return NULL;
                    }

                    write_big_endian((uint16_t) number, (unsigned char*) &output_buffer[line_counter]);
                    line_counter++;
                    continue;
                }

                case INVALID_pseudoinstruction: {
                    caller.line_number = __LINE__ - 1;
                    error.message = "Pseudo instruction missing - it should not reach here";
                    log_stderr(caller, error, FATAL);
                    free(output_buffer);
                    return NULL;
                }

                default: {
                    caller.line_number = __LINE__ - 1;
                    error.message = "Pseudo instruction missing - it should not reach here";
                    log_stderr(caller, error, FATAL);
                    free(output_buffer);
                    return NULL;
                }
            }
        }

        caller.line_number = __LINE__ - 1;
        error.message = "Invalid instruction";
        error.character_number = strstr(line, token) - line + 1;
        log_stderr(caller, error, ERROR);
        free(output_buffer);
        return NULL;
    }
    caller.line_number = __LINE__ - 1;
    error.message = "Reached EOF but no END instruction";
    error.line_number = file_line;
    error.character_number = strlen(line);
    log_stderr(caller, error, ERROR);
    free(output_buffer);
    return NULL;
}

unsigned char * assemble(char input[], int *output_length) {
    int line_count = 1;
    linked_char_list *linked_lines = NULL;

    char *start = input;
    for (int i = 0;; i++) {
        int end = input[i] == '\0' ? 1 : 0;
        if (input[i] == '\n' || input[i] == '\0') {
            line_count = input[i] == '\0' ? line_count : line_count + 1;
            input[i] = '\0';
            linked_char_list *new_line = malloc(sizeof(linked_char_list));
            new_line->vale = start;
            new_line->next = linked_lines;
            linked_lines = new_line;
            start = &input[i + 1];
        }
        if (end) {
            break;
        }
    }

    // make an array
    char **lines = malloc(sizeof(char *) * (line_count));
    for (int i = line_count - 1; linked_lines != NULL; i--) {
        lines[i] = linked_lines->vale;
        linked_char_list *next = linked_lines->next;
        free(linked_lines);
        linked_lines = next;
    }

    // Do work
    int exit_code = 0;
    if (first_pass(line_count, lines) != 0) {
        exit_code = 1;
    }

    uint16_t output_size = 0;
    unsigned char *object = NULL;
    if (exit_code == 0) {
        object = second_pass(line_count, lines, &output_size);
    }

    free(lines);
    free_labels();

    if (object != NULL) {
        *output_length = output_size;
    }
    return object;
}

void write_big_endian(uint16_t number, unsigned char *output) {
    output[0] = (unsigned char) (number >> 8);
    output[1] = (unsigned char) (number & 0xFF);
}
