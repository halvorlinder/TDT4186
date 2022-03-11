#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//How many clients can wait for a response at the same time
#define MAX_QUEUE_SIZE 5

void setResponse(char* response, char* wwwpath) {
    //Clear response
    memset(response, 0, 8000);

    //Set header
    strcat(response, "HTTP/0.9 200 OK\r\n\n");

    //Set body
    FILE *page = fopen(wwwpath, "r");
    char line[100];
    while (fgets(line, 100, page)) {
        strcat(response, line);
    }
}


int main( int argc, char *argv[] )  {
    if( argc != 3 ) {
        printf("Two arguments expected.\n");
        exit(0);
    }

    char* wwwpath = argv[1];
    int port = atoi(argv[2]);
    printf("Arguments: %s and %d\n", wwwpath, port);

    //Create a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    //Setup address: localhost:port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Bind the address to the socket
    bind(serverSocket, (struct sockaddr *) &address, sizeof(address));

    //Start listening👂
    int listening = listen(serverSocket, MAX_QUEUE_SIZE);
    if (listening < 0) {
        printf("Listening failed. Exitting\n");
        return 1;
    }
    printf("Listening at localhost:%d\n", port);
    
    int clientSocket;
    char response[8000];
    while(1) {
        //Send response to client
        clientSocket = accept(serverSocket, NULL, NULL);
        // char request[200];
        // recv(clientSocket, request, sizeof(request), 0);
        // printf("Request: %s", request);
        setResponse(response, wwwpath);
        send(clientSocket, response, sizeof(response), 0);
        close(clientSocket);
    }
    return 0;
}