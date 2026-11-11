//
// Created by botnet64 on 11/11/26.
//

#ifndef ASSEMBLER_ERROR_HANDLER_H
#define ASSEMBLER_ERROR_HANDLER_H

typedef struct {
    char *section_name;
    char *subsection_name;
    int line_number;
    int character_number;
} caller_details;

typedef struct {
    char *message;
    char *text;
    int line_number;
    int character_number;
} error_details;

enum log_type {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4,
    NONE = 5,
};

void set_log_level(enum log_type level);

void log_stderr(caller_details caller, error_details error, enum log_type type);

#endif //ASSEMBLER_ERROR_HANDLER_H