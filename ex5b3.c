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

    connect_to_shared_mem(&key_q, &shm_id_q, &shm_ptr_q, P_ARR_SIZE, 'p');
    connect_to_shared_mem(&key_p, &shm_id_p, &shm_ptr_p, Q_ARR_SIZE, 'q');
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
    scanf("%d\n", &num);

    shm_ptr_p[P_NUM] = num;

    kill(shm_ptr_p[P_PID], SIGUSR1);

    pause();
    get_res_p();
}

//------------------------------------------------
// maybe not ?
void q_request()
{
    int num, i;

    for(i = Q_START_NUM; i < Q_RES - 1; i++)
    {
        scanf("%d\n", &num);

        if(num == 0)
        {
            shm_ptr_q[i] = 0;
            break;
        }
        else
            shm_ptr_q[i] = num;
    }

    kill(shm_ptr_q[Q_PID], SIGUSR2);

    pause();
    get_res_q();
}

//------------------------------------------------

void get_res_p()
{
    int res = shm_ptr_p[P_RES];
    printf("%s Prime\n", res ? "Is" : "Is not a ");
}

//------------------------------------------------

void get_res_q()
{
    int res = shm_ptr_q[Q_RES];
    printf("%s Palindrome\n", res ? "Is " : "Is not a ");
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
