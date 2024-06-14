/*
 * This is a file that implements the operation on TCP sockets that are used by
 * all of the programs used in this assignment.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "connection.h"

//I have included the following libraries.
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int tcp_connect( char* hostname, int port )
{
    //Vi oppretter vår socket her. 
    int ourSocketConnect = socket(AF_INET, SOCK_STREAM, 0);
    //Dersom den ikke blir opprettet så sendes feilmeldingen ut
    //om at den ikke blir det. 
    if(ourSocketConnect < 0){
        fprintf(stderr, "Feil ved ourSocket\n");
        return -1;
    }

    //Spesifiserer hvilke port og serveradresse som skal bli koblet til. 
    //vi begynner med å initialisere serv_addr til 0. 
    struct sockaddr_in serveradresse = {0};
    struct in_addr addresse;
    serveradresse.sin_family = AF_INET;
    serveradresse.sin_port = htons(port);
    serveradresse.sin_addr = addresse;

    if (inet_aton(hostname, &addresse) == 0) { 
        fprintf(stderr, "feil adresse\n"); 
        return -1;
    }

    //Connecter så til servern ved bruk av connect funksjonen som tar tre argumenter
    if (connect(ourSocketConnect, (struct sockaddr*) &serveradresse, sizeof(serveradresse)) < 0){
        fprintf(stderr, "Feil ved tilkoblingen\n");
        return -1;
    }
    return ourSocketConnect;
}

int tcp_read( int sock, char* buffer, int n )
{
    //Bruker funksjonen read til å lese over bytesa og setter dem over
    //til minnet og i dette tilfellet "leser". gjør den til ssize_t så 
    //slipper jeg å caste.
    ssize_t leser = 0;

    //setter opp en loop som vil kjøre til den ikke når det maximale antallet bytes
    //bruker ikke =< for da vil det kanskje være for mye bytes i bufferet.

    ssize_t leserIN = recv(sock, buffer + leser, n - leser, 0);
    if(leserIN == 0){
        perror("Socketen er lukket\n");
        return 0;
    } else if(leser < 0){
        perror("feil med lesinga fra bufferet til socketen");
        return -1;
    } else{
        leser += leserIN;
    }
    
    return leser;
}

int tcp_write( int sock, char* buffer, int bytes )
{   
    //Har kontroll på antallet bytes som har blitt skrevet til socketen, 
    //gjør den til ssize_t for da slipper jeg å caste
    ssize_t bytesSkalSendes = 0;
    
    //Lager beholder her for å ha kontroll på antall bytes som blir sendt
    while(bytesSkalSendes < bytes){
        ssize_t beholder = send(sock, buffer + bytesSkalSendes, bytes - bytesSkalSendes, 0);
        if (beholder < 0){
            perror ("Feil med skrivingen fra bufferet til socketen");
            return -1;
        } else{
            bytesSkalSendes += beholder;
        }
    }
    return bytesSkalSendes;
}

int tcp_write_loop( int sock, char* buffer, int bytes )
{   
    //Holder styr på ant bytes som har blitt skrevet på socketen
    int antBytesSkrevet = 0; 
    //Loopen her vil fortsette til ant bytes vil være lik bytes
    while(antBytesSkrevet < bytes){
        //Loopen kjører tcp_write metoden for å kunne skrive så mye bytes som mulig
        int antBytesILoop = tcp_write(sock, buffer + antBytesSkrevet, bytes - antBytesSkrevet);
        if (antBytesILoop < 0){
            perror("Feil med skrivingen fra bufferet til socketen i loopen");
            return -1;
        } else{
            antBytesSkrevet += antBytesILoop;
        }
    }
    return antBytesSkrevet;
}

void tcp_close( int sock )
{
    //Lukker socketen 
    close(sock);
}

int tcp_create_and_listen( int port )
{
    //Vi lager en socket som skal lytte
    int ourSocket2Listen = socket(AF_INET, SOCK_STREAM, 0);
    if (ourSocket2Listen < 0){
        perror("Har ikke klart å lage socketen\n");
        return -1;
    }

    //Setter socketen til lokale adressen og porten
    //struct sockaddr_in serveradresse = {0};
    struct sockaddr_in serveradresse;
    serveradresse.sin_family = AF_INET;
    serveradresse.sin_port = htons(port);
    serveradresse.sin_addr.s_addr = INADDR_ANY;

    //Bruker funksjonenen bind() her til å sette listen socketen vår til porten for at
    //den skal kunne klare å lytte til kommende connection
    if(bind(ourSocket2Listen, (struct sockaddr*) &serveradresse, sizeof(struct sockaddr_in))){
        perror("klarte ikke å binde");
        return -1;
    }

    //Vi satt 5 som max antallet kø av forbindelser som er i vente
    //Her så gjør vi det til en møteplass. 
    if(listen(ourSocket2Listen, 5) < 0){
        perror("klarte ikke å lytte");
        close(ourSocket2Listen);
        return -1;
    }
    return ourSocket2Listen;
}

int tcp_accept( int server_sock )
{
    //Vi beholder clientsock adressen her
    struct sockaddr_in clientserver;
    //bevarer str her som vi senere sender som pointer inn i accept().
    socklen_t clientserverStr = sizeof(clientserver);
    //bruker da accept til å kunne koble klienten. 
    int ourSocket3Accept = accept(server_sock, (struct sockaddr*) &clientserver, &clientserverStr);
    if (ourSocket3Accept < 0){
        perror("klarte ikke å godkjenne");
        close(ourSocket3Accept);
        return -1;
    }
    return ourSocket3Accept;
}

int tcp_wait( fd_set* waiting_set, int wait_end )
{   
    //iterrerer gjennom filene og finner filen med størst
    /*int ourSocket4Wait = -1;
    for(int i = 0; i < wait_end; i++){
        if(FD_ISSET(i, waiting_set)){
            ourSocket4Wait = i > ourSocket4Wait ? i : ourSocket4Wait;
        }
    }

    if(ourSocket4Wait < 0){
        perror("ingen sockets tilgjengelig");
        return -1;
    }*/

    //Kaller på select, setter de tre siste argumentene til NULL, i å med
    //at vi ikke trenger noe der. 
    int venting = select(wait_end+1, waiting_set, NULL, NULL, NULL);
    if(venting < 0){
        perror("Klarte ikke å bruke select");
        return -1;
    }
    return venting;
}

int tcp_wait_timeout( fd_set* waiting_set, int wait_end, int seconds )
{   
    //Her bruker vi timeval til å kunne sette timeouten til selecten.
    //Setter opp en socket som vil kunne klare å hold på høyst som da
    //vi vil kunne vente på. 
    struct timeval tVal;
    int ourSocket4Wait = -1;
    for (int i = 0; i < wait_end; i++){
        if( waiting_set != NULL && FD_ISSET(i, waiting_set)){
            ourSocket4Wait = i > ourSocket4Wait ? i : ourSocket4Wait;
        }
    }

    //Error dersom vi ikke finner noe. 
    if (ourSocket4Wait < 0){
        perror("ingen sockets ledige");
        return -1;
    }

    //Lager kopi av pekern til waiting_set 
    fd_set beholder = *waiting_set;
    tVal.tv_sec = seconds;
    tVal.tv_usec = 0;
    
    //setter opp select, som vil da vente på at aktivitetene bli ferdig
    int venting = select(ourSocket4Wait + 1, waiting_set, NULL, NULL, &tVal);
    if(venting < 0){
        perror("klarte ikke å gjennomføre");
        return -1;
    }
    
    return 0; 
}

 