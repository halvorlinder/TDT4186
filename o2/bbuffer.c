#ifndef ____BBUFFER___H___
#define ____BBUFFER___H___
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "sem.h"

/*
 * Bounded Buffer implementation to manage int values that supports multiple
 * readers and writers.
 *
 * The bbuffer module uses the sem module API to synchronize concurrent access
 * of readers and writers to the bounded buffer.
 */

/* Opaque type of a Bounded Buffer.
 * ...you need to figure out the contents of struct BNDBUF yourself
 */

typedef struct BNDBUF
{
    unsigned int *buffer_list;
    unsigned int head;
    unsigned int tail;
    SEM *access_sem;
    SEM *full_sem;
    SEM *empty_sem;
    unsigned int size;
} BNDBUF;

/* Creates a new Bounded Buffer.
 *
 * This function creates a new bounded buffer and all the helper data
 * structures required by the buffer, including semaphores for
 * synchronization. If an error occurs during the initialization the
 * implementation shall free all resources already allocated by then.
 *
 * Parameters:
 *
 * size     The number of integers that can be stored in the bounded buffer.
 *
 * Returns:
 *
 * handle for the created bounded buffer, or NULL if an error occured.
 */

BNDBUF *bb_init(unsigned int size)
{
    BNDBUF* buff = malloc(sizeof(BNDBUF));
    unsigned int *buffer_list = malloc(size);
    buff->buffer_list = buffer_list;
    buff->access_sem = sem_init(1);
    buff->full_sem = sem_init(size);
    buff->empty_sem = sem_init(0);
    buff->head = 0;
    buff->tail = 0;
    buff->size = size;
    return buff;
}

/* Destroys a Bounded Buffer.
 *
 * All resources associated with the bounded buffer are released.
 *
 * Parameters:
 *
 * bb       Handle of the bounded buffer that shall be freed.
 */

void bb_del(BNDBUF *bb)
{
    free(bb->buffer_list);
    free(bb->access_sem);
    free(bb->full_sem);
    free(bb->empty_sem);
    free(bb);
}

/* Retrieve an element from the bounded buffer.
 *
 * This function removes an element from the bounded buffer. If the bounded
 * buffer is empty, the function blocks until an element is added to the
 * buffer.
 *
 * Parameters:
 *
 * bb         Handle of the bounded buffer.
 *
 * Returns:
 *
 * the int element
 */

int bb_get(BNDBUF *bb)
{
    P(bb->empty_sem);
    P(bb->access_sem);
    int socket = bb->buffer_list[bb->head];
    bb->head = (bb->head + 1) % bb->size;
    V(bb->full_sem);
    V(bb->access_sem);
    return socket;
}

/* Add an element to the bounded buffer.
 *
 * This function adds an element to the bounded buffer. If the bounded
 * buffer is full, the function blocks until an element is removed from
 * the buffer.
 *
 * Parameters:
 *
 * bb     Handle of the bounded buffer.
 * fd     Value that shall be added to the buffer.
 *
 * Returns:
 *
 * the int element
 */

void bb_add(BNDBUF *bb, int fd)
{
    P(bb->full_sem);
    P(bb->access_sem);
    bb->buffer_list[bb->tail] = fd;
    bb->tail = (bb->tail + 1) % bb->size;
    V(bb->empty_sem);
    V(bb->access_sem);
}

#endif
