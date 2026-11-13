//
// Created by botnet64 on 11/10/26.
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <core/assembler.h>
#include <core/symbol_table.h>

int main(void) {
    FILE *file = fopen("../test.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    char buffer[2048];
    int i = 0;
    while (1) {
        char c = (char) fgetc(file);
        if (c == EOF) {
            buffer[i++] = '\0';
            break;
        }
        buffer[i++] = c;
    }
    fclose(file);
    int j = 0;
    unsigned char *output = assemble(buffer, &j);

    FILE *out = fopen("a.out", "wb");
    fwrite(output, 1, j, out);
    fclose(out);
    free(output);

    return 0;
}
