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

// The maximum size of a given file path
#define MAX_PATH_SIZE 200

// The maximum size of a HTTP response
#define MAX_RESPONSE_SIZE 8000

/*!
 * Set the content of the HTTP response that is to be sent
 *
 * @param response is the HTTP response we wish to set
 * @param wwwpath is the path to the index.html file, containing the page to be set in the response
 */
void set_response(char *response, char *wwwpath)
{
    // Clear response
    memset(response, 0, MAX_RESPONSE_SIZE);

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

/*!
 * Extract and set the file path from a given HTTP request
 *
 * @param request is the HTTP request containing the path
 * @param path is the referance to the path we wish to update accordingly to our given request
 */
void set_path(char *request, char *path)
{
    int length = 0;
    int counting = 0;
    memset(path, 0, MAX_PATH_SIZE);
    for (int i = 0; i < MAX_PATH_SIZE; i++)
    {
        // char request_i = request[i];
        if (!counting && request[i] == ' ')
        {
            counting = 1;
        }
        else if (counting)
        {
            if (request[i] == ' ' || request[i] == '\n' || request[i] == '\r')
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
    char response[MAX_RESPONSE_SIZE];
    char path[MAX_PATH_SIZE];
    char request[MAX_PATH_SIZE];
    char full_path[MAX_PATH_SIZE];
    BNDBUF* bb = m.buffer;
    const char* wwwpath = m.wwwpath;
    int clientSocket;
    while (1)
    {
        clientSocket = bb_get(bb);
        recv(clientSocket, request, sizeof(request), 0);
        set_path(request, path);
        memset(full_path, '\0', MAX_PATH_SIZE);
        strcat(full_path, wwwpath);
        strcat(full_path, path);

        set_response(response, full_path);
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
    while (1)
    {
        clientSocket = accept(serverSocket, NULL, NULL);
        bb_add(bb, clientSocket);
    }

    return 0;
}
