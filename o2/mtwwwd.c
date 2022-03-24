#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "bbuffer.h"
#include <pthread.h>

// How many clients can wait for a response at the same time
#define MAX_QUEUE_SIZE 5

void setResponse(char *response, char *wwwpath)
{
    // Clear response
    memset(response, 0, 8000);

    // char* thread_id;
    // sprintf(thread_id, "%ld", pthread_self());
    // strcat(response, thread_id);

    //Set body
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

typedef struct THREAD_MESSAGE {
    BNDBUF *buffer;
    const char *wwwpath;
} THREAD_MESSAGE;

void* worker_function(void *message)
{
    struct THREAD_MESSAGE* mp = ((struct THREAD_MESSAGE*) message);
    struct THREAD_MESSAGE m = *mp; 
    char response[8000];
    char path[200];
    char request[200];
    char full_path[200];
    BNDBUF* bb = m.buffer;
    const char* wwwpath = m.wwwpath;
    int clientSocket;
    while (1)
    {
        clientSocket = bb_get(bb);
        recv(clientSocket, request, sizeof(request), 0);
        set_path(request, path);
        memset(full_path, '\0', 200);
        strcat(full_path, wwwpath);
        strcat(full_path, path);

        //For some reason we get a segfault when this line is removed
        puts(path);

        setResponse(response, full_path);
        send(clientSocket, response, strlen(response), 0);
        close(clientSocket);
    }
}


int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Four arguments expected.\n");
        exit(0);
    }

    const char *wwwpath = argv[1];
    const int port = atoi(argv[2]);
    const int N_THREADS = atoi(argv[3]);
    const int buffer_slots = atoi(argv[4]);

    // Create a ring buffer
    struct BNDBUF* bb = bb_init(buffer_slots);
    
    struct THREAD_MESSAGE message = {bb, wwwpath};
    // Create threads
    pthread_t thread_ids[N_THREADS];
    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_create(&thread_ids[i], NULL, worker_function, (void*)&message);
    }

    // Create a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Setup address: localhost:port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

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

    while (1)
    {
        clientSocket = accept(serverSocket, NULL, NULL);
        bb_add(bb, clientSocket);
    }

    return 0;
}
