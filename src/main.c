#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <core/assemble.h>

#define NAME "massembler"
#define VERSION "1.0.0"
#define RETURN_CODE_SUCCESS 0
#define RETURN_CODE_INVALID_USAGE 1
#define RETURN_CODE_INVALID_ARGUMENT 2
#define RETURN_CODE_OUT_OF_MEMORY 3
#define RETURN_CODE_ASSEMBLING_FAILED 4
#define RETURN_CODE_IO_FAILED 5

void print_usage(const char *name, FILE *stream);

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        print_usage(NAME, stderr);
        return RETURN_CODE_INVALID_USAGE;
    }
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_usage(NAME, stdout);
        return 0;
    }
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        printf("%s v%s\nBy BotNet6420\n", NAME, VERSION);
        return 0;
    }

    const char *input_filename = argv[1];
    const char *output_file_name = "program.bin";

    if (argc == 3) {
        output_file_name = argv[2];
    }
    if (argc > 3) {
        print_usage(NAME, stderr);
        return RETURN_CODE_INVALID_ARGUMENT;
    }

    if (strcmp(input_filename, output_file_name) == 0) {
        fprintf(stderr, "Input and output file names cannot be the same\n");
        return RETURN_CODE_IO_FAILED;
    }

    // Open input
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", input_filename);
        return RETURN_CODE_IO_FAILED;
    }
    // Find input character count
    int char_count = 0;
    while (fgetc(input_file) != EOF) {
        char_count++;
    }
    if (ferror(input_file)) {
        fprintf(stderr, "Error reading input file: %s\n", input_filename);
        fclose(input_file);
        return RETURN_CODE_IO_FAILED;
    }
    // Allocate memory for input
    char *buffer = malloc(char_count + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation for input failed\n");
        fclose(input_file);
        return RETURN_CODE_OUT_OF_MEMORY;
    }
    // Read input
    rewind(input_file);
    for (int i = 0; i < char_count; i++) {
        buffer[i] = (char) fgetc(input_file);
    }
    buffer[char_count] = '\0';
    fclose(input_file);

    // Generate binary
    int binary_length = -1;
    unsigned char *binary = assemble(buffer, &binary_length);
    free(buffer);
    if (binary == NULL || binary_length < 0) {
        fprintf(stderr, "Assembling failed\n");
        return RETURN_CODE_ASSEMBLING_FAILED;
    }

    // Open output
    FILE *output_file = fopen(output_file_name, "wb");
    if (output_file == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", output_file_name);
        free(binary);
        return RETURN_CODE_IO_FAILED;
    }
    // Write output
    if (fwrite(binary, 1, (size_t) binary_length, output_file) != (size_t) binary_length) {
        fprintf(stderr, "Error writing output file: %s\n", output_file_name);
        fclose(output_file);
        free(binary);
        return RETURN_CODE_IO_FAILED;
    }
    // Close output
    if (fclose(output_file) == EOF) {
        fprintf(stderr, "Error closing output file: %s\n", output_file_name);
        free(binary);
        return RETURN_CODE_IO_FAILED;
    }

    free(binary);

    return RETURN_CODE_SUCCESS;
}

void print_usage(const char *name, FILE *stream) {
    fprintf(stream, "Usage: %s [OPTIONS] <INPUT> [OUTPUT]\n", name);
    fprintf(stream, "Convert INPUT source code into Mano's Basic Computer binary instruction.\n");
    fprintf(stream, "If OUTPUT is specified, overwrites the file with the data.\n");
    fprintf(stream, "Options:\n");
    fprintf(stream, "  -h, --help    Print this help message and exit\n");
    fprintf(stream, "  -v, --version Print version information and exit\n");
}
