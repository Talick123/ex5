/*

 - create array of size ARR_SIZE = 1000
 - cell  0 in shared memory = getpid()
 - cell  1, 2, 3 = 0
 - cell 4 = 1 (the "lock")
 - all other cells = 0
 - goes to sleep until signal received
 - when wakes up, prints a. how many different values in array
                         b. min value in array
                         c. max value in array
 - releases the memory, finishes

 ftok(".", '5')

*/

// --------include section------------------------

#include <iostream>
#include <stdio.h>
#include <unistd.h> //for pause
#include <errno.h>
#include <stdlib.h>

#define ARR_SIZE 1000
#define LOCK 4

// --------const and enum section------------------------

enum STATE {LOCKED, UNLOCKED};
const int MAX_NUM = 1000;

// --------prototype section------------------------

void catch_sig1(int signum);
void create_shared_mem(key_t *key, int *shm_id, int *shm_ptr);
void init_data(int *shm_ptr);
void read_and_print_data(int *shm_ptr);
void reset_arr(int arr[]);
void close_shared_mem(int *shm_id, struct shmid_ds *shm_desc);
void perrorandexit(char *msg);

// --------main section------------------------

int main()
{
    key_t key;
    int shm_id;
    int *shm_ptr;
    struct shmid_ds shm_desc;

    signal(SIGUSR1, catch_sig1);

    create_shared_mem(&key, &shm_id, shm_ptr);
    init_data(shm_ptr);
    sleep();
    read_and_print_data(shm_ptr);
    close_shared_mem(&shm_id, &shm_desc);

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void catch_sig1(int signum){}

//-------------------------------------------------

void create_shared_mem(key_t *key, int *shm_id,int *shm_ptr)
{
    *key = ftok(".", '5');
    if(*key == -1)
        perrorandexit("ftok failed");

    if((*shm_id = shmget(key, ARR_SIZE, IPC_CREAT | 0600)) == -1)
        perrorandexit("shmget failed");

    shm_ptr = (int*)shmat(smh_id, NULL, 0);
    if (!shm_ptr)
        perrorandexit("shmat failed");
}

//-------------------------------------------------

void init_data(int *shm_ptr)
{
    int index;

    shm_ptr[0] = getpid();

    for(index = 1; index < ARR_SIZE, index++)
        shm_ptr[index] = 0;

    shm_ptr[LOCK] = UNLOCKED;
}

//-------------------------------------------------

void read_and_print_data(int *shm_ptr)
{
    int index, counter = 0, max = 0, min = MAX_NUM;
    int arr[ARR_SIZE];
    reset_arr(arr);

    for(index = 0; index < ARR_SIZE; index++)
    {
        arr[index]++;
        if(shm_ptr[index] < min)
            min = shm_ptr[index];
        if(shm_ptr[index] > max)
            max = shm_ptr[index];
    }

    for(index = 0; index < ARR_SIZE; index++)
        if(arr[index] != 0)
            counter++;

    printf("The number of different primes received is: %d\n", counter);
	printf("The max prime is: %d. The min prime is: %d\n", max, min);
}

//-------------------------------------------------

void reset_arr(int arr[])
{
	int index;
	for(index = 0; index < ARR_SIZE; index++)
		arr[index] = 0;
}

//-------------------------------------------------

void close_shared_mem(int *shm_id, struct shmid_ds *shm_desc)
{
    if(shmctl(shm_id, IPC_RMID, shm_desc) == -1)
        perrorandexit("shmctl failed");
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
