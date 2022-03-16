#include <pthread.h>
/*
 * Semaphore implementation for the synchronization of POSIX threads.
 *
 * This module implements counting P/V semaphores suitable for the 
 * synchronization of POSIX threads. POSIX mutexes and condition variables 
 * shall be utilized to implement the semaphore operations.
 */

/* Opaque type of a semaphore. 
 * ...you need to figure out the contents of struct SEM yourself!
 */
typedef struct SEM {
    pthread_mutex_t count_lock;
    pthread_cond_t count_nonzero;
    unsigned int count;
}SEM;

/* Creates a new semaphore.
 *
 * This function creates a new semaphore. If an error occurs during the 
 * initialization, the implementation shall free all resources already 
 * allocated so far.
 *
 * Parameters:
 *
 * initVal      the initial value of the semaphore
 *
 * Returns:
 *
 * handle for the created semaphore, or NULL if an error occured.
 */
SEM *sem_init(int initVal){
    SEM sem;
    if(pthread_mutex_init(&sem.count_lock, NULL) != 0) {
        return NULL;
    }
    if(pthread_cond_init(&sem.count_nonzero, NULL) != 0){
        return NULL;
    }
    sem.count = initVal;
    return &sem; 
}

/* Destroys a semaphore and frees all associated resources.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to destroy
 *
 * Returns:
 *
 * 0 on success, negative value on error. 
 *
 * In case of an error, not all resources may have been freed, but 
 * nevertheless the semaphore handle must not be used any more.
 */
int sem_del(SEM *sem){
    if(pthread_mutex_destroy(&sem->count_lock) != 0) {
        return -1;
    }
    if(pthread_cond_destroy(&sem->count_nonzero) != 0) {
        return -1;
    }
    return 0;
}

/* P (wait) operation.
 * 
 * Attempts to decrement the semaphore value by 1. If the semaphore value 
 * is 0, the operation blocks until a V operation increments the value and 
 * the P operation succeeds.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to decrement
 */
void P(SEM *sem){
    pthread_mutex_lock(&sem->count_lock);
    while (sem->count == 0)
        pthread_cond_wait(&sem->count_nonzero, &sem->count_lock);
    sem->count -= 1;
    pthread_mutex_unlock(&sem->count_lock);
}

/* V (signal) operation.
 *
 * Increments the semaphore value by 1 and notifies P operations that are 
 * blocked on the semaphore of the change.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to increment
 */
void V(SEM *sem){
    pthread_mutex_lock(&sem->count_lock);
    if (sem->count == 0)
        pthread_cond_signal(&sem->count_nonzero);
    sem->count += 1;
    pthread_mutex_unlock(&sem->count_lock);
}