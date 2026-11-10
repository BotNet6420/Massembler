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
    uint16_t file_counter = 0; // to be used by error messages to report the problematic line

    while (fgets(line, LINE_LENGTH, input) != NULL) {
        file_counter++;
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
            if (sscanf(part2, " %x %c", &number, &dummy) != 1 || strtok(NULL, " \t\r\n") != NULL) {
                // TODO: ERROR(invalid address)
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
