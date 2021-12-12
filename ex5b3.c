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

// client

#define P_ARR_SIZE 4
#define Q_ARR_SIZE 22

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


enum shm_index_p {P_PID = 0, P_CL_PID = 1, P_NUM = 2, P_RES = 3};
enum shm_index_q {Q_PID = 0, Q_CL_PID = 1, Q_START_NUM = 2, Q_RES = 21};
enum res {FALSE, TRUE};
int *shm_ptr_q, *shm_ptr_p;

// --------prototype section------------------------

void connect_to_shared_mem(key_t *key, int *shm_id, int **shm_ptr, int size, char type);
void read_data();
void p_request();
void q_request();
void get_res_p();
void get_res_q();
void catch_sig2(int signum);
void catch_sig1(int signum);
void catch_sigint(int signum);
void perrorandexit(char *msg);

// --------main section------------------------

int main()
{
    key_t key_q, key_p;
    int shm_id_q, shm_id_p;

    signal(SIGUSR1, catch_sig1);
    signal(SIGUSR2, catch_sig2);

    //connect to both shared memory - prime and palindrome
    connect_to_shared_mem(&key_q, &shm_id_q, &shm_ptr_q, Q_ARR_SIZE, 'q');
    connect_to_shared_mem(&key_p, &shm_id_p, &shm_ptr_p, P_ARR_SIZE, 'p');
    read_data();

    return EXIT_SUCCESS;
}

//------------------------------------------------

void connect_to_shared_mem(key_t *key, int *shm_id, int **shm_ptr, int size, char type)
{
    *key = ftok(".", type);
    if(*key == -1)
        perrorandexit("ftok failed");

    if((*shm_id = shmget(*key, size, 0600)) == -1)
        perrorandexit("shmget failed");

    *shm_ptr = (int*)shmat(*shm_id, NULL, 0);
    if (!(*shm_ptr))
        perrorandexit("shmat failed");
}

//------------------------------------------------

void read_data()
{
    shm_ptr_p[P_CL_PID] = getpid();
    shm_ptr_q[Q_CL_PID] = getpid();

    // init pid
    char c;
    while(true)
    {
        c = getchar();

        switch (c) {
            //read num to check is prime
            case 'p':
                p_request();
                break;
            //read series of num to check is palindrome
            case 'q':
                q_request();
                break;
            default:
                break;
        }
    }
}

//------------------------------------------------

void p_request()
{
    int num;

    scanf(" %d", &num);
    getchar();

    shm_ptr_p[P_NUM] = num;

    kill(shm_ptr_p[P_PID], SIGUSR1);
    pause();
    get_res_p();
}

//------------------------------------------------
void q_request()
{
    int num, i;

    for(i = Q_START_NUM; i < Q_RES - 1; i++)
    {
        scanf(" %d", &num);

        if(num == 0)
        {
            shm_ptr_q[i] = 0;
            break;
        }
        else
            shm_ptr_q[i] = num;
    }
	getchar();
    kill(shm_ptr_q[Q_PID], SIGUSR2);

    pause();
    get_res_q();
}

//------------------------------------------------

void get_res_p()
{
    int result = shm_ptr_p[P_RES];
    printf("%s Prime\n", result ? "Is" : "Is not a ");
}

//------------------------------------------------

void get_res_q()
{
    int result = shm_ptr_q[Q_RES];
    printf("%s Palindrome\n", result ? "Is " : "Is not a ");
}

//-------------------------------------------------

void catch_sig2(int signum)
{
    signal(SIGUSR2, catch_sig2);
}

//-------------------------------------------------

void catch_sig1(int signum)
{
    signal(SIGUSR1, catch_sig1);
}

//-------------------------------------------------

void catch_sigint(int signum)
{
    shmdt(shm_ptr_p);
    shmdt(shm_ptr_q);
    exit(EXIT_SUCCESS);
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
