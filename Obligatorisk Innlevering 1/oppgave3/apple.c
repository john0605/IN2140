#include <stdio.h>
#include "the_apple.c"

int locateworm(char* apple);
int removeworm(char* apple);
int main()
{
    //Printer bare ut hva funksjonene returnerer.
    printf("Start index of worm: %d\n", locateworm(apple));
    printf("Length of worm: %d\n", removeworm(apple));
    return 0;
}

//leter til den kommer til første char som er del av ormen og returnerer bare indeksen
int locateworm(char* apple) {
    char chosenletter = *apple;
    int index = 0;
    while(chosenletter == 'a' || chosenletter == 'p' || chosenletter == 'l' || chosenletter == 'e' || 
    chosenletter == ' ' || chosenletter == '\n') {
        apple++;
        index++;
        chosenletter = *apple;
    }
    return index;
}

//leter seg foerst fram til ormen som i locateworm(), saa bytter den bokstavene til ormen med mellomrom
int removeworm(char* apple) {
    char chosenletter = *apple;
    while(chosenletter == 'a' || chosenletter == 'p' || chosenletter == 'l' || chosenletter == 'e' || 
    chosenletter == ' ' || chosenletter == '\n') {
        apple++;
        chosenletter = *apple;
    }

    int removedbytes = 0;
    while(chosenletter == 'w' || chosenletter == 'o' || chosenletter == 'r' || chosenletter == 'm' || 
    chosenletter == '\n' || chosenletter == ' ') {
        if (!(chosenletter == '\n' || chosenletter == ' '))
        {
            *apple = ' ';
            removedbytes++;
        }
        apple++;
        chosenletter = *apple;
    }
    return removedbytes;
}
