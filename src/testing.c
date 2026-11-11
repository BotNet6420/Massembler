//
// Created by botnet64 on 11/10/26.
//

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <core/assembler.h>
#include <core/symbol_table.h>

int main(void) {
    printf("Hello, World!\n");
    FILE *file = fopen("/home/botnet64/Documents/projects/mano-basic-computer/assembler/test.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    first_pass(file);
    dump_labels();
    rewind(file);
    second_pass(file);
    fclose(file);

    return 0;
}
