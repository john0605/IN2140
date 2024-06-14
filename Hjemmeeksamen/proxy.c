/*
 * This is the main program for the proxy, which receives connections for sending and receiving clients
 * both in binary and XML format. Many clients can be connected at the same time. The proxy implements
 * an event loop.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "xmlfile.h"
#include "connection.h"
#include "record.h"
#include "recordToFormat.h"
#include "recordFromFormat.h"
#include <arpa/inet.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>


/* This struct should contain the information that you want
 * keep for one connected client.
 */
struct Client
{
    int sock;
    char id;
    char isXmlClient;

};

typedef struct Client Client;
int antallKlienter = 0;

void usage( char* cmd )
{
    fprintf( stderr, "Usage: %s <port>\n"
                     "       This is the proxy server. It takes as imput the port where it accepts connections\n"
                     "       from \"xmlSender\", \"binSender\" and \"anyReceiver\" applications.\n"
                     "       <port> - a 16-bit integer in host byte order identifying the proxy server's port\n"
                     "\n",
                     cmd );
    exit( -1 );
}

/*
 * This function is called when a new connection is noticed on the server
 * socket.
 * The proxy accepts a new connection and creates the relevant data structures.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */

//endrer på parameteren her, slik at jeg kan bare sette den nye klienten inn i listen. 
void handleNewClient( int server_sock, struct Client*** klientListe )
{
    int rc;
    int sock_fd;

    sock_fd = tcp_accept(server_sock);
    if(sock_fd<0){
        fprintf(stderr, "feil med accept\n");
        exit(-1);
    }

    //leser hvordan typen clienten er
    char clientType;
    rc = tcp_read(sock_fd, &clientType, 1);
    if (rc < 0){
        fprintf(stderr, "feil ved typen\n");
        exit(-1);
    }
    
    //leser clientens ID
    char clientId;
    rc = tcp_read(sock_fd, &clientId, 1);
    if (rc < 0){
        fprintf(stderr, "feil ved read\n");
        exit(-1);
    }

    //initializerer strukturen til clienten
    Client* newClient = malloc(sizeof(Client));
    if(newClient == NULL){
        fprintf(stderr, "problemer med allokering av minne");
        exit(-1);
    }

    newClient->isXmlClient = clientType;
    newClient->sock = sock_fd;
    newClient->id = clientId;

    //Reallocer her dersom det trengs mere plass
    if (antallKlienter > 0){
        *klientListe = realloc(*klientListe, sizeof(struct Client*)*(antallKlienter + 1));
    }

    (*klientListe)[antallKlienter] = newClient;
    antallKlienter++;
    //free(newClient);

    /*Tester for å sjekke om jeg får riktig verdi. (test a.xml)
    printf("xml or not: %c\n\n", newClient->isXmlClient);
    printf("ID: %c\n\n", newClient->id);
    printf("sock: %d\n\n", newClient->sock);    
    */
}

/*
 * This function is called when a connection is broken by one of the connecting
 * clients. Data structures are clean up and resources that are no longer needed
 * are released.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void removeClient( Client* client )
{
    if (client == NULL){
        return;
    }

    tcp_close(client->sock);
    antallKlienter--;
    free(client);
}

/*
 * This function is called when the proxy received enough data from a sending
 * client to create a Record. The 'dest' field of the Record determines the
 * client to which the proxy should send this Record.
 *
 * If no such client is connected to the proxy, the Record is discarded without
 * error. Resources are released as appropriate.
 *
 * If such a client is connected, this functions find the correct socket for
 * sending to that client, and determines if the Record must be converted to
 * XML format or to binary format for sendig to that client.
 *
 * It does then send the converted messages.
 * Finally, this function deletes the Record before returning.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void forwardMessage( Record* msg, struct Client **klientListe )
{   
    //sjekker om destinasjonen til proxyen er tilkoblet, slik at vi får koblet til recieveren. 
    for(int i = 0; i<antallKlienter; i++){
        if(msg->dest == klientListe[i]->id){
            int bytes = 0;
            if(klientListe[i]->isXmlClient == 'X'){
                char *buffer;
                buffer = recordToXML(msg, &bytes);
                tcp_write(klientListe[i]->sock, buffer, bytes );
            }else{
                char *buffer;
                buffer = recordToBinary(msg, &bytes);
                tcp_write(klientListe[i]->sock, buffer, bytes );
            }
        }
    }
    //clearRecord(msg);
    deleteRecord(msg);
    //free(msg);
}

/*
 * This function is called whenever activity is noticed on a connected socket,
 * and that socket is associated with a client. This can be sending client
 * or a receiving client.
 *
 * The calling function finds the Client structure for the socket where acticity
 * has occurred and calls this function.
 *
 * If this function receives data that completes a record, it creates an internal
 * Record data structure on the heap and calls forwardMessage() with this Record.
 *
 * If this function notices that a client has disconnected, it calls removeClient()
 * to release the resources associated with it.
 *
 * *** The parameters and return values of this functions can be changed. ***
 */
void handleClient( Client* client, struct Client **klientListe )
{
    char buffer [1028];
    int rc; 

    rc = tcp_read(client->sock, buffer, 1028);

    if (rc <= 0){
        removeClient(client);
        return;
    }

    buffer[rc]  = "\0";
    Record *record;

    int antallLestBytes = 0;
    if(client->isXmlClient == 'X'){
        record = XMLtoRecord(buffer, 1028, &antallLestBytes);
    }
    else{
        record = BinaryToRecord(buffer, 2048, &antallLestBytes);
    }

    //sender den 
    forwardMessage(record, klientListe);
}

int main( int argc, char* argv[] )
{
    int port;
    int server_sock;

    if( argc != 2 )
    {
        usage( argv[0] );
    }

    port = atoi( argv[1] );
    server_sock = tcp_create_and_listen( port );
    if( server_sock < 0 ) exit( -1 );

    /* add your initialization code */
    fd_set fds;
    int antFD; 

    //Setter opp en liste som vil holde på tilkoblede klienter
    struct Client **klientListe = malloc(sizeof(struct Client*));
    if(klientListe == NULL){
        fprintf(stderr, "klarte ikke å malloce");
        exit(EXIT_FAILURE);
    }
    
    /*
     * The following part is the event loop of the proxy. It waits for new connections,
     * new data arriving on existing connection, and events that indicate that a client
     * has disconnected.
     *
     * This function uses handleNewClient() when activity is seen on the server socket
     * and handleClient() when activity is seen on the socket of an existing connection.
     *
     * The loops ends when no clients are connected any more.
     */
    do
    {
        FD_ZERO(&fds);
        FD_SET(server_sock, &fds);
        antFD = server_sock;

        for (int i = 0; i < antallKlienter; i++){
            FD_SET(klientListe[i]->sock, &fds);
            if(klientListe[i]->sock > antFD){
                antFD = klientListe[i]->sock;
            }
        }
        tcp_wait(&fds, antFD);

        //hvis aktivitet på server sock
        if (FD_ISSET(server_sock, &fds)){
            handleNewClient(server_sock, &klientListe);
            
            /*tester om den holder på klientene
            printf("antal klienter: %d", antallKlienter);
            fflush(stdout);*/
        }

        //sjekker om det er noe aktivitet på noen av klientene i listen
        for (int i = 0; i < antallKlienter; i++){
            if(FD_ISSET(klientListe[i]->sock, &fds)){

                /*tester for å se om den sendes til xmlrecord
                //printf("det er aktivitet paa: %c\n\n\n", klientListe[i]->id);*/
                //fflush(stdout);

                handleClient(klientListe[i], klientListe);
            }
        }
    }
    while( antallKlienter!= 0 );
    //free(klientListe);
    tcp_close( server_sock );

    return 0;
}