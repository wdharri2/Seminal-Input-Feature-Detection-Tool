#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    int a = 0;
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    } else if(a == 1){
        int i = 0;
        i++;
    } else{
        int i = 0;
        i++;
    }

    while(1){
        break;
    }

    // File opened successfully
    fclose(file);
    return 0;
}
