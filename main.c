#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("Hello, World!\n");
    FILE* file = fopen("/home/botnet64/Documents/projects/mano-basic-computer/assembler/test.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    first_pass(file);
    fclose(file);
    return 0;
}

