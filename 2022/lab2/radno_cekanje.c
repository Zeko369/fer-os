#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Expected 1 argument with number of numbers to generate");
        return 1;
    }

    int count = atoi(argv[1]);

    return 0;
}
