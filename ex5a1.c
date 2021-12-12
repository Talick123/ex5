/*


File: ex5a1.c ex5a2.c
Generate and Collect Primes from Shared Memory
=====================================================================
Written by: Tali Kalev, ID:208629691, Login: talikal
		and	Noga Levy, ID:315260927, Login: levyno

This program runs with 4 different processes. Three processes that generates
random numbers, when the number is prime the process sends it to main process adds to shared memory.
When the filling processes sees that the shared memory is full, it sends signal to main
program, prints out data and ends. The main proccess wakes, counts how many different
primes are in shared memory, finds the smallest and biggest number, prints, closes
shared memory and ends.

Compile: gcc -Wall ex5a1.c -o ex5a1
         gcc -Wall ex5a2.c -o ex5a2
     (ex5a1 = main process, ex5a2 = sub process)

Run: for start run the main process.
    Then, run 3 times the sub processes and send to the vector
    arguments the number of process (1-3):
        ./ex5a1
        ./ex5a2 1
        ./ex5a2 2
        ./ex5a2 3

Input: No Input

Output:
    From main process (ex5a1) = minimum prime, max prime and number of
    different numbers in the array.
    Example: The number of different primes received is: 168
             The max prime is: 997. The min primes is: 2
    From sub process (ex5a2) = prime number they send the most to main process
    Example: Process 1101373 sent the prime 233, 14 times

*/

// --------include section------------------------

#include <signal.h>
#include <stdio.h>
#include <unistd.h> //for pause
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

#define ARR_SIZE 1000
#define LOCK 4
#define START 5

// --------const and enum section------------------------

enum STATE {LOCKED, UNLOCKED};

// --------prototype section------------------------

void catch_sig1(int signum);
void create_shared_mem(int *shm_id, int **shm_ptr);
void init_data(int *shm_ptr);
void read_and_print_data(int *shm_ptr);
bool new_in_shm(int prime, int curr_ind, int *shm_ptr);
void close_shared_mem(int *shm_id, int *shm_ptr);
void perrorandexit(char *msg);

// --------main section------------------------

int main()
{
	int shm_id;
    int *shm_ptr;

    signal(SIGUSR1, catch_sig1);

    create_shared_mem(&shm_id, &shm_ptr); //create shared memmory array
    init_data(shm_ptr);
    pause();
    read_and_print_data(shm_ptr);
    close_shared_mem(&shm_id, shm_ptr);

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void catch_sig1(int signum){}

//-------------------------------------------------

void create_shared_mem(int *shm_id, int **shm_ptr)
{
    key_t key;
    key = ftok(".", '5');
    if(key == -1)
        perrorandexit("ftok failed");

    if((*shm_id = shmget(key, ARR_SIZE, IPC_CREAT | 0600)) == -1)
        perrorandexit("shmget failed");

    *shm_ptr = (int*)shmat(*shm_id, NULL, 0);
    if (!(*shm_ptr))
        perrorandexit("shmat failed");

    printf("Shm_id: %d\n", *shm_id);
}

//-------------------------------------------------
// get shm_ptr and reset it
void init_data(int *shm_ptr)
{
    int index;

    shm_ptr[0] = getpid();

    for(index = 1; index < ARR_SIZE; index++)
        shm_ptr[index] = 0;

    shm_ptr[LOCK] = UNLOCKED;
}

//-------------------------------------------------

void read_and_print_data(int *shm_ptr)
{
    int index, counter = 1, max = shm_ptr[START], min = shm_ptr[START];

    for(index = START + 1; index < ARR_SIZE; index++)
    {
        if(new_in_shm(shm_ptr[index], index, shm_ptr))
            counter++;
        if(shm_ptr[index] < min)
            min = shm_ptr[index];
        if(shm_ptr[index] > max)
            max = shm_ptr[index];
    }

    printf("The number of different primes received is: %d\n", counter);
	printf("The max prime is: %d. The min prime is: %d\n", max, min);
}

//-------------------------------------------------
// gets prime and shared memmory and check if new in array
bool new_in_shm(int prime, int curr_ind, int *shm_ptr)
{
    int index;

    for(index = START; index < curr_ind; index++)
        if(prime == shm_ptr[index])
            return false;

    return true;
}

//-------------------------------------------------

void close_shared_mem(int *shm_id, int *shm_ptr)
{
	shmdt(shm_ptr);

    if(shmctl(*shm_id, IPC_RMID, NULL) == -1)
        perrorandexit("shmctl failed");
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
