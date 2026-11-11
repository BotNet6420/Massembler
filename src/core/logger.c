//
// Created by botnet64 on 11/11/26.
//

#include "logger.h"

#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 1024

enum log_type log_level = INFO;

void set_log_level(const enum log_type level) {
    log_level = level;
}

/**
 * WARNING: BUFFERS MAY OVERFLOW GIVEN LARGE ENOUGH INPUTS (~1000 characters) USE AT YOUR OWN RISK
 * @param caller caller information
 * @param error error information
 * @param type severity of the log
 */
void log_stderr(const caller_details caller, const error_details error, const enum log_type type) {
    // name.subname:line:character - Level: message
    // line_no | text
    //         | ---^

    if (type < log_level || type == NONE) {
        return;
    }

    const char *level_name;
    const int level_name_length = 7;

    switch (type) {
        case DEBUG:
            level_name = "DEBUG";
            break;
        case INFO:
            level_name = "INFO";
            break;
        case WARNING:
            level_name = "WARNING";
            break;
        case ERROR:
            level_name = "ERROR";
            break;
        case FATAL:
            level_name = "FATAL";
            break;
        default:
            level_name = "LOG";
    }


    char error_message[BUFFER_SIZE] = "";
    int index = 0;

    // Add section name
    if (caller.section_name != NULL) {
        const unsigned long length = strlen(caller.section_name);

        if (length > 0 && length < BUFFER_SIZE / 4) {
            for (int i = 0; i < length; i++) {
                error_message[index++] = caller.section_name[i];
            }
            if (caller.subsection_name != NULL) {
                error_message[index++] = '.';
            } else if (caller.line_number > 0) {
                error_message[index++] = ':';
            }
        }
    }

    // Add subsection name
    if (caller.subsection_name != NULL) {
        const unsigned long length = strlen(caller.subsection_name);

        if (length > 0 && length < BUFFER_SIZE / 4) {
            for (int i = 0; i < length; i++) {
                error_message[index++] = caller.subsection_name[i];
            }
            if (caller.line_number > 0) {
                error_message[index++] = ':';
            }
        }
    }

    // Add line number
    if (caller.line_number > 0) {
        char line[BUFFER_SIZE];
        sprintf(line, "%d", caller.line_number);
        const unsigned long length = strlen(line);
        for (int i = 0; i < length; i++) {
            error_message[index++] = line[i];
        }
        if (caller.character_number > 0) {
            error_message[index++] = ':';
        }
    }

    // Add character number
    if (caller.line_number > 0 && caller.character_number > 0) {
        char character[BUFFER_SIZE];
        sprintf(character, "%d", caller.character_number);
        const unsigned long length = strlen(character);
        for (int i = 0; i < length; i++) {
            error_message[index++] = character[i];
        }
    }

    error_message[index++] = ' ';
    error_message[index++] = '-';
    error_message[index++] = ' ';

    // Add level name
    // random scope so length doesn't get re-declared/shadowed
    {
        const unsigned long length = strlen(level_name);
        for (int i = 0; i < length; i++) {
            error_message[index++] = level_name[i];
        }
        for (int i = 0; i < level_name_length - length; i++) {
            error_message[index++] = ' ';
        }
    }

    // Add error message
    if (error.message != NULL) {
        error_message[index++] = ':';
        error_message[index++] = ' ';

        const unsigned long length = strlen(error.message);
        for (int i = 0; i < length && length < BUFFER_SIZE / 4; i++) {
            error_message[index++] = error.message[i];
        }
    }

    // close error line
    error_message[index] = '\0';
    fprintf(stderr, "%s\n", error_message);


    // handle text
    if (error.text == NULL) {
        return;
    }
    if (strlen(error.text) > BUFFER_SIZE / 2) {
        return;
    }

    // Create text line
    char text_line[BUFFER_SIZE] = "   ";
    index = 3;
    unsigned long line_number_character_length = 0;
    char line_number_text[BUFFER_SIZE] = "";

    // get line number length
    if (error.line_number > 0) {
        sprintf(line_number_text, "%d", error.line_number);
        line_number_character_length = strlen(line_number_text);
    }

    // Append number
    if (line_number_character_length > 0) {
        for (int i = 0; i < line_number_character_length; i++) {
            text_line[index++] = line_number_text[i];
        }
    }

    text_line[index++] = ' ';
    text_line[index++] = '|';
    text_line[index++] = ' ';

    // append text
    if (error.text != NULL) {
        const unsigned long length = strlen(error.text);
        for (int i = 0; i < length && error.text[i] != '\n'; i++) {
            text_line[index++] = error.text[i];
        }
    }
    text_line[index] = '\0';
    fprintf(stderr, "%s\n", text_line);


    // Create pointer line
    if (error.character_number <= 0 || error.character_number > 255) {
        return;
    }

    char pointer_line[BUFFER_SIZE] = "   ";
    index = 3;
    for (int i = 0; i < line_number_character_length; i++) {
        pointer_line[index++] = ' ';
    }

    pointer_line[index++] = ' ';
    pointer_line[index++] = '|';
    pointer_line[index++] = ' ';

    for (int i = 0; i < error.character_number - 1; i++) {
        pointer_line[index++] = '-';
    }
    pointer_line[index++] = '^';
    pointer_line[index] = '\0';
    fprintf(stderr, "%s\n", pointer_line);
}
