/*
Noga: not suure how to end program with SIGINT ? how we can close the shared memory

    FRONT END PROCESS

    - in an infinite loop:
        - reads either p or q from user
            - if read p, reads number to check primeness
            - if reads q, reads series of numbers until digit 0 (not including 0)
                to check palindromiut (you can assume that the series of digits
                is not too long)
        - puts input into the appropriate shared memory
        - sends signal to appropriate server
        - goes to sleep until receives signal SIGUSR1/SIGUSR2
        - when client wakes from sleep, prints answer to screen

    - program ends when SIGINT received

    ftok(".", 'p') for primeness
    ftok(".", 'q') for palindromiut
*/

#define P_ARR_SIZE 4
#define Q_ARR_SIZE 4

// --------include section------------------------

#include <signal.h>
#include <stdio.h>
#include <unistd.h> //for pause
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// --------const and enum section------------------------

enum shm_index_p {P_PID = 0, P_CL_PID = 1, P_NUM = 2, P_RES = 3};
enum shm_index_q {Q_PID = 0, Q_CL_PID = 1, Q_START_NUM = 2, Q_RES = 21};
enum res {FALSE, TRUE};

// --------prototype section------------------------

void connect_to_shared_mem(key_t *key, int *shm_id, int **shm_ptr);
void init_data(int *shm_ptr)
void close_shared_mem(int *shm_id, struct shmid_ds *shm_desc);

int stop = 0;

// --------main section------------------------

int main()
{
    //Noga: need to save key and id ?
    //Noga: I think we can create them inside the connect/create shared mem function (?)
    key_t key_q, key_p;
    int shm_id_q, shm_id_p;
    int *shm_ptr_q, *shm_ptr_p;

    struct shmid_ds shm_desc; // Noga: mm?

    signal(SIGUSR1, catch_sig1);
    signal(SIGUSR2, catch_sig2);

    connect_to_shared_mem(&key_q, &shm_id_q, &shm_ptr_q, P_ARR_SIZE);
    connect_to_shared_mem(&key_p, &shm_id_p, &shm_ptr_p, Q_ARR_SIZE);
    read_data(shm_ptr);

    return EXIT_SUCCESS;
}

//------------------------------------------------

void connect_to_shared_mem(key_t *key, int *shm_id, int **shm_ptr, int size)
{
    *key = ftok(".", '5');
    if(*key == -1)
        perrorandexit("ftok failed");

    if((*shm_id = shmget(*key, size, 0600)) == -1)
        perrorandexit("shmget failed");

    *shm_ptr = (int*)shmat(*shm_id, NULL, 0);
    if (!(*shm_ptr))
        perrorandexit("shmat failed");
}

//------------------------------------------------

void read_data(int *shm_ptr_p, int *shm_ptr_q)
{
    char c;
    while(true)
    {
        c = getc();
        switch (c) {
            //read num to check is prime
            case 'p':
                p_request(shm_ptr_p);
                pause();
                get_res_p(shm_ptr_p, P_RES);
                break;
            //read series of num to check is palindrome
            case 'q':
                q_request(shm_ptr_q);
                pause();
                get_res_q(shm_ptr_q, Q_RES);
                break;
            default:
                break;
        }
    }
}

//------------------------------------------------

void p_request(int *shm_ptr_p)
{
    int num;
    scanf("%d\n", &num);

    shm_id_p[P_CL_PID] = getpid();
    shm_id_p[P_NUM] = num;

    kill(shm_id_p[P_PID], SIGUSR1)
}

//------------------------------------------------
// maybe not ?
void q_request(int *shm_ptr_q)
{
    int num, i;
    bool get_more = true;

    for(i = Q_START_NUM; i < Q_RES; i++)
    {
        if (get_more)
            scanf("%d\n", &num);
        if(num == 0)
        {
            get_more = false;
            shm_id_q[i] = 0;
        }
        else
            shm_id_q[i] = num;
    }

    shm_id_p[Q_CL_PID] = getpid();

    kill(shm_id_q[q_PID], SIGUSR2)
}

//------------------------------------------------

int get_res_p(int *shm_ptr_p)
{
    int res = shm_id_p[P_RES];
    printf("%sPrime", res ? "" : "not ")
}

//------------------------------------------------

int get_res_q(int *shm_ptr_q)
{
    int res = shm_id_p[Q_RES];
    printf("%sPalindrome", res ? "" : "not ")
}

void catch_sig2(int signum) {}
void catch_sig1(int signum) {}
void catch_sigint(int signum)
{
    exit(EXIT_SUCCESS);
}
