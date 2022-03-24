#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// How many clients can wait for a response at the same time
#define MAX_QUEUE_SIZE 5

void setResponse(char *response, char *wwwpath)
{
    // Clear response
    memset(response, 0, 8000);

    // Set body
    FILE *page = fopen(wwwpath, "r");
    if (page)
    {
        char line[100];
        while (fgets(line, 100, page))
        {
            strcat(response, line);
        }
    }
    else
    {
        strcat(response, "404 NOT FOUND");
    }
}

void set_path(char *request, char *path)
{
    int length = 0;
    int counting = 0;
    memset(path, 0, 200);
    for (int i = 0; i < 200; i++)
    {
        if (!counting && request[i] == ' ')
        {
            counting = 1;
            continue;
        }
        else if (counting)
        {
            if (request[i] == ' ' || request[i] == '\n' || request[i] == '\r\n')
            {
                return;
            }
            path[length] = request[i];
            length++;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Two arguments expected.\n");
        exit(0);
    }

    char *wwwpath = argv[1];
    int port = atoi(argv[2]);

    // Create a TCP socket
    int serverSocket = socket(AF_INET6, SOCK_STREAM, 0);

    // Setup address: localhost:port
    struct sockaddr_in6 address;
    address.sin6_family = AF_INET6;
    address.sin6_port = htons(port);
    address.sin6_addr = in6addr_loopback;

    // Bind the address to the socket
    bind(serverSocket, (struct sockaddr *)&address, sizeof(address));

    // Start listening👂
    int listening = listen(serverSocket, MAX_QUEUE_SIZE);
    if (listening < 0)
    {
        printf("Listening failed. Exitting\n");
        return 1;
    }
    printf("Listening at localhost:%d\n", port);

    int clientSocket;
    char response[8000];
    char path[200];
    char request[200];
    char full_path[200];
    while (1)
    {
        // Send response to client
        clientSocket = accept(serverSocket, NULL, NULL);
        recv(clientSocket, request, sizeof(request), 0);
        printf("\n\n%s\n\n", request);
        set_path(request, path);
        puts(path);
        memset(full_path, '\0', 200);
        strcat(full_path, wwwpath);
        strcat(full_path, path);
        puts(full_path);
        setResponse(response, full_path);
        send(clientSocket, response, strlen(response), 0);
        close(clientSocket);
    }
    return 0;
}