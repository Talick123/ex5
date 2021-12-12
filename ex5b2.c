/*

File: ex5b1.c ex5b2.c ex5b3.c
Interactions Between Prime Checker, Palindrome Checker and Client
=====================================================================
Written by: Tali Kalev, ID:208629691, Login: talikal
		and	Noga Levy, ID:315260927, Login: levyno

ex5b1: creates shared memory, goes to sleep, wakes up via SIGUSR1, checks
number inputed into shared memory, puts into shared memory if num is prime or not.
ex5b2: creates shared memory, goes to sleep, wakes up via SIGUSR2, checks series
of numbers inputed into shared memory, puts into shared memory if series is palindrome
or not.
ex5b3: connects to both shared memories created. reads from user p or q, and then
number of series of numbers respectively. Enters into the appropriate shared memory
the data and waits for response. Prints result onto screen.

Compile: gcc -Wall ex5b1.c -o ex5b1
         gcc -Wall ex5b2.c -o ex5b2
        gcc -Wall ex5b3.c -o ex5b3
     (ex5b1 = prime checker, ex5b2 = palindrome checker, ex5b3 = client)

Run: run all three programs one after the other
        ./ex5b1
        ./ex5b2
        ./ex5b3

Input: Only ex5b3 receives input. If input is p and then number, ex5b1 checks if
        it is prime. If input is q and then a series of numbers ending in 0, ex5b2
        checks if it is a palindrome.
        Example: p
                 7

Output: ex5c3 prints answer of other programs appropriately
        Example:
        Is Prime

*/

//palindrome server

#define ARR_SIZE 22

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

// --------const and enum section------------------------

enum shm_index {PID = 0, CL_PID = 1, START_NUM = 2, RES = 21};
enum res {FALSE, TRUE};
int shm_id;
int *shm_ptr;

// --------prototype section------------------------

void create_shared_mem();
void init_data();
void handle_requests();
void catch_sig2(int signum);
void catch_sigint(int signum);
int is_pal(int num);
void perrorandexit(char *msg);

// --------main section------------------------

int main()
{
    signal(SIGUSR2, catch_sig2);
    signal(SIGINT, catch_sigint);

    create_shared_mem();
    init_data();
    handle_requests();

    return EXIT_SUCCESS;
}

//----------------------------------------------------

void create_shared_mem()
{
    key_t key;

    key = ftok(".", 'q');
    if(key == -1)
        perrorandexit("ftok failed");

    if((shm_id = shmget(key, ARR_SIZE, IPC_CREAT | 0600)) == -1)
        perrorandexit("shmget failed");

    shm_ptr = (int*)shmat(shm_id, NULL, 0);
    if (!shm_ptr)
        perrorandexit("shmat failed");
}

//-------------------------------------------------

void init_data()
{
    int index;

    //i = 0 curr pid
    shm_ptr[PID] = getpid();

    // the rest is 0 as default
    for(index = 1; index < ARR_SIZE; index++)
        shm_ptr[index] = 0;
}

//-------------------------------------------------

void handle_requests()
{
    int i;
    int size;

    for(;;)
    {
		size = 0;
        pause();

        for(i = 0; i < RES - START_NUM; i++)
        {
            if(shm_ptr[START_NUM + i] == 0)
                break;
            size++;
        }

        shm_ptr[RES] = is_pal(size);
        kill(shm_ptr[CL_PID], SIGUSR2);
    }
}

//-------------------------------------------------

void catch_sig2(int signum)
{
    signal(SIGUSR2, catch_sig2);
}

//-------------------------------------------------

void catch_sigint(int signum)
{
    shmdt(shm_ptr);

    if(shmctl(shm_id, IPC_RMID, NULL) == -1)
        perrorandexit("shmctl failed");

    exit(EXIT_SUCCESS);
}

//-------------------------------------------------

int is_pal(int n)
{
    // Initialise flag to zero.
    int flag = 1;

    // Loop till array size n/2.
    for (int i = 0; i <= n / 2 && n != 0; i++)
    {
        // Check if first and last element are different
        // Then set flag to 1.
        if (shm_ptr[START_NUM + i] != shm_ptr[(START_NUM + n - 1)- i ]) {
            flag = 0;
            break;
        }
    }

    // If flag is set then print Not Palindrome
    // else print Palindrome.
    return flag;
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
