#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//2b
int stringsum(char* s) {
    int sum = 0;
    int storrelse = strlen(s);
    char string[storrelse];
    strcpy(string, s);

    //setter alle bokstaver i arrayet til lowercase fordi det gjoer det lettere
    for (int i = 0; i < storrelse; i++) {
        string[i] = tolower(string[i]);
        char ascii = (int) string[i];
        if (!((ascii - 96) < 0)) {
            sum = sum + (ascii - 96);
        } 
        else if (ascii == 32) {

        } 
        else {
            return -1;
        } 
    }   
    return sum;
}

//2c
int distance_between(char* s, char c) {
    char *start = index(s, c);
    char *slutt = rindex(s, c);
    if (!start) {
        return -1;
    }
    else {
        return slutt - start;
    }
}

//2d
char* string_between(char* s, char c) {
    char *start = index(s, c);
    char *slutt = rindex(s, c);
    int distanse = 0;
    char *svar = NULL;
    int indeks = (start - s) + 1;

    //Sjekker om det finnes en char c.
    if (start) {
        distanse = slutt - start;
        /*
        Hvis det faktisk er en streng mellom de to bokstavene vil det vaere nok plass
        til en nullterminatoren saa lenge vi allokerer distanse som minne. Da kan vi bare bruke
        memcpy for aa kopiere over det vi vil ha med. Er derimot distanse 0, altsaa at
        det kun er en bokstav av den typen i strengen, vil det ikke vaere nok plass til
        en nullterminatoren, og vi maa derfor allokere en ekstra plass til denne. 
        */
        if (distanse > 0) {
            svar = (char *) malloc(distanse);
            memcpy(svar, &s[indeks], (distanse - 1));
            svar[distanse - 1] = '\0';
        } else {  
            svar = (char *) malloc(distanse + 1);
            strcpy(svar, "");
        }
    }
    return svar;
}

//2e
int stringsum2(char* s, int* res) {
    *res = 0;
    int sum = 0;
    int storrelse = strlen(s);
    char string[storrelse];
    strcpy(string, s);

    //setter alle bokstaver i arrayet til lowercase fordi det gjoer det lettere
    for (int i = 0; i < storrelse; i++) {
        string[i] = tolower(string[i]);
        char ascii = (int) string[i];
        if (!((ascii - 96) < 0)) {
            sum = sum + (ascii - 96);
        } 
        else if (ascii == 32) {

        } 
        else {
            *res = -1;
            return -1;
        } 
    }
    *res = sum;
    if (sum > 0) {
        return 0;
    } else {
        return -1;
    }
}
