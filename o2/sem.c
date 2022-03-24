#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

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
typedef struct SEM
{
    pthread_mutex_t count_lock;
    pthread_cond_t count_nonzero;
    unsigned int count;
} SEM;

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
SEM *sem_init(int initVal)
{
    SEM* sem = (SEM*)malloc(sizeof(SEM));
    if (pthread_mutex_init(&(sem->count_lock), NULL) != 0)
    {
        return NULL;
    }
    if (pthread_cond_init(&(sem->count_nonzero), NULL) != 0)
    {
        return NULL;
    }
    sem->count = initVal;
    return sem;
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
int sem_del(SEM *sem)
{
    if (pthread_mutex_destroy(&sem->count_lock) != 0)
    {
        return -1;
    }
    if (pthread_cond_destroy(&sem->count_nonzero) != 0)
    {
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
void P(SEM *sem)
{
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
void V(SEM *sem)
{
    pthread_mutex_lock(&sem->count_lock);
    if (sem->count == 0)
        pthread_cond_signal(&sem->count_nonzero);
    sem->count += 1;
    pthread_mutex_unlock(&sem->count_lock);
}


// The code below is test code to ensure that the semaphores are working correctly

// typedef struct s{
//     long* sum_ptr;
//     SEM* sem;
// } s;
// void* worker_function(void* s_ptr){
//     s* str = (s*)(s_ptr);
//     long* sum = str->sum_ptr;
//     SEM* sem = str->sem;
//     for(int i = 0; i<10000; i++){
//         P(sem);
//         long sum2 = *sum;
//         sum2+=i;
//         *sum = sum2;
//         V(sem);
//     }
//     return NULL;
// }


// int main(int argc, char *argv[]){
//     long sum = 0;
//     int n = 20;
//     SEM* sem = sem_init(1);
//     s str = {&sum, sem};
//     pthread_t thread_ids[n];
//     for (int i = 0; i < n; i++)
//     {
//         pthread_create(&thread_ids[i], NULL, worker_function, &str);
//     }
//     for (int i = 0; i < n; i++)
//        pthread_join(thread_ids[i], NULL);
    
//     printf("%ld\n",sum);
//     return 0;
// }