//
// Created by botnet64 on 11/10/26.
//

#include <core/assembler.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <core/symbol_table.h>

#include "logger.h"


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
    caller_details caller = {
        .section_name = "assembler",
        .subsection_name = "first_pass",
    };

    error_details error = {};

    while (fgets(line, LINE_LENGTH, input) != NULL) {
        file_line++;
        if (strchr(line, '\n') == NULL && feof(input) == 0) {
            caller.line_number = __LINE__ - 1;
            error.line_number = file_line;
            error.message = "Source line too long; cannot parse";
            error.text = NULL;
            log_stderr(caller, error, ERROR);
            return 1;
        }

        error.line_number = file_line;
        error.text = line;
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


    *mem_cell = opcode_binary_code | address_binary_code | indirection_binary_code;
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

    *mem_cell = opcode_binary_code;
    return 0;
}

int second_pass(FILE *input) {
    caller_details caller = {
        .section_name = "assembler",
        .subsection_name = "second_pass",
    };

    error_details error = {};

    char line[LINE_LENGTH];
    uint16_t output_buffer[4096] = {0};

    uint16_t line_counter = 0;
    int file_line = 0;

    while (fgets(line, LINE_LENGTH, input) != NULL) {
        file_line++;
        error.line_number = file_line;

        if (strchr(line, '\n') == NULL && feof(input) == 0) {
            caller.line_number = __LINE__ - 1;
            error.message = "Source line too long; cannot parse";
            log_stderr(caller, error, ERROR);
            return 1;
        }

        char copy[LINE_LENGTH];
        strcpy(copy, line);
        const char *instruction = line;
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
                return 1;
            }

            // ORG and END cannot have labels
            if (strcmp(token, "ORG") == 0) {
                caller.line_number = __LINE__ - 1;
                error.message = "ORG instruction cannot have a label";
                error.character_number = strstr(line, label) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }
            if (strcmp(token, "END") == 0) {
                caller.line_number = __LINE__ - 1;
                error.message = "END instruction cannot have a label";
                error.character_number = strstr(line, label) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
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
                caller.line_number = __LINE__ - 1;
                char m[128] = "Instruction address too big";
                char n[128];
                sprintf(n, " (%04x)", line_counter);
                strcat(m, n);
                error.message = m;
                error.character_number = strstr(line, token) - line + 1;
                log_stderr(caller, error, ERROR);
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
                        caller.line_number = __LINE__ - 1;
                        error.message = "ORG instruction missing address operand";
                        error.character_number = strstr(line, token) - line + 5;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // try parse the number
                    unsigned int number = 0;
                    if (sscanf(token2, " %x", &number) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse HEX address";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    int n = 0;
                    if (sscanf(token2, " %x %n%c", &number, &n, &dummy) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse HEX address";
                        error.character_number = strstr(line, token2) - line + n + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Expected newline";
                        error.character_number = strstr(line, token3) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // address out of bounds
                    if (number > 0xfff) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "ORG address too big";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
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
                        return 1;
                    }

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
                caller.line_number = __LINE__ - 1;
                char m[128] = "Instruction address too big";
                char n[128];
                sprintf(n, " (%04x)", line_counter);
                strcat(m, n);
                error.message = m;
                error.character_number = strstr(line, token) - line + 1;
                log_stderr(caller, error, ERROR);
                return 1;
            }
            switch (get_pseudoinstruction(token)) {
                case DEC: {
                    if (token2 == NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "DEC instruction missing operand";
                        error.character_number = strstr(line, token) - line + 5;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // try parse the number
                    int number = 0;
                    if (sscanf(token2, " %d", &number) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse decimal number";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    int n = 0;
                    if (sscanf(token2, " %d %n%c", &number, &n, &dummy) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse decimal number";
                        error.character_number = strstr(line, token2) - line + n + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Expected newline";
                        error.character_number = strstr(line, token3) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    if (number > INT16_MAX) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "decimal number too big";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    if (number < INT16_MIN) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "decimal number too small";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    output_buffer[line_counter] = (uint16_t) number;
                    line_counter++;
                    continue;
                }

                case HEX: {
                    if (token2 == NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "HEX instruction missing operand";
                        error.character_number = strstr(line, token) - line + 5;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // try parse the number
                    unsigned int number = 0;
                    if (sscanf(token2, " %x", &number) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse hexadecimal number";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // catch syntax errors (trailing non-whitespace character)
                    char dummy;
                    int n = 0;
                    if (sscanf(token2, " %x %n%c", &number, &n, &dummy) != 1) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Could not parse hexadecimal number";
                        error.character_number = strstr(line, token2) - line + n + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    // catch invalid extra token
                    if (token3 != NULL) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "Expected newline";
                        error.character_number = strstr(line, token3) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    if (number > UINT16_MAX) {
                        caller.line_number = __LINE__ - 1;
                        error.message = "hexadecimal number too big";
                        error.character_number = strstr(line, token2) - line + 1;
                        log_stderr(caller, error, ERROR);
                        return 1;
                    }

                    output_buffer[line_counter] = (uint16_t) number;
                    line_counter++;
                    continue;
                }

                case INVALID_pseudoinstruction: {
                    caller.line_number = __LINE__ - 1;
                    error.message = "Pseudo instruction missing - it should not reach here";
                    log_stderr(caller, error, FATAL);
                    return 1;
                }

                default: {
                    caller.line_number = __LINE__ - 1;
                    error.message = "Pseudo instruction missing - it should not reach here";
                    log_stderr(caller, error, FATAL);
                    return 1;
                }
            }
        }

        caller.line_number = __LINE__ - 1;
        error.message = "Invalid instruction";
        error.character_number = strstr(line, token) - line + 1;
        log_stderr(caller, error, ERROR);
        return 1;
    }
    caller.line_number = __LINE__ - 1;
    error.message = "Reached EOF but no END instruction";
    error.line_number = file_line;
    error.character_number = strlen(line);
    log_stderr(caller, error, ERROR);
    return 1;
}
