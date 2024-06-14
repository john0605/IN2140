#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    char setning[strlen(argv[argc-2])];
    strcpy(setning, argv[argc-2]);
    char bokstav = argv[argc-1][0];
    int lengde = strlen(setning);

    for (int i = 0; i < lengde; i++) {
        if(setning[i] == 'a' || setning[i] == 'e' || setning[i] == 'i' || setning[i] == 'o' || setning[i] == 'u' ||
        setning[i] == 'A' || setning[i] == 'E' || setning[i] == 'I' || setning[i] == 'O' || setning[i] == 'U'){
            setning[i] = bokstav;
        }
    }

    printf("%s\n", setning);
    return 0;
}
