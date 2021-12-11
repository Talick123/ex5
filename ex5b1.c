/*
Noga: not suure how to end program with SIGINT ? how we can close the shared memory

  PRIMES SERVER

  - allocates shared memory with 4 cells
  - cell 0 = getpid();
  - cell 1 = client will enter its pid
  - cell 2 = client will enter number to check its primenizatzia
  - cell 3 = server will enter answer to primenizatzia (1 or 0)

  - server runs in an infinite loop
  - in the loop the server:
        - goes to sleep until receive SIGUSR1
        - after receiving signal, wakes, checks primenizatzia of number
        in cell in 2
        - puts answer in cell 3
        - sends SIGUSR1 to client

    - program ends when SIGINT received  (release memory also)

    ftok(".", 'p')

*/

#define ARR_SIZE 4

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

enum shm_index {PID, CL_PID, NUM, RES};
enum res {FALSE, TRUE};

// --------prototype section------------------------

void create_shared_mem(key_t *key, int *shm_id, int **shm_ptr);
void init_data(int *shm_ptr);
void handle_requests(int *shm_ptr);
void catch_sig1(int signum);
void catch_sigint(int signum);
int is_prime(int num);
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
    signal(SIGINT, catch_sigint);

    create_shared_mem(&key, &shm_id, &shm_ptr);
    init_data(shm_ptr);
    handle_requests(shm_ptr);
    close_shared_mem(&shm_id, &shm_desc);


    return EXIT_SUCCESS;
}

//----------------------------------------------------

void create_shared_mem(key_t *key, int *shm_id, int **shm_ptr)
{
    *key = ftok(".", 'p'); // Noga: q or p ? Tali: p
    if(*key == -1)
        perrorandexit("ftok failed");

    if((*shm_id = shmget(*key, ARR_SIZE, IPC_CREAT | 0600)) == -1)
        perrorandexit("shmget failed");

    *shm_ptr = (int*)shmat(*shm_id, NULL, 0);
    if (!(*shm_ptr))
        perrorandexit("shmat failed");
}

//-------------------------------------------------

void init_data(int *shm_ptr)
{
    int index;

    //i = 0 curr pid
    shm_ptr[PID] = getpid();

    // the rest is 0 as default
    for(index = 1; index < ARR_SIZE; index++) //Tali : Necessary?
        shm_ptr[index] = 0;
}

//-------------------------------------------------

void handle_requests(int *shm_ptr)
{
    //Tali: changed cause when signal received, true = false;
    while(1) //Tali: for some reason didnt like true??
    {
        for(;;)
        {
            pause();
            shm_ptr[RES] = is_prime(shm_ptr[NUM]);
            kill(shm_ptr[CL_PID], SIGUSR1);
        }
    }
}

//-------------------------------------------------

void catch_sig1(int signum) {}

void catch_sigint(int signum)
{
    exit(EXIT_SUCCESS);
}
//-------------------------------------------------

void close_shared_mem(int *shm_id, struct shmid_ds *shm_desc)
{
    if(shmctl(*shm_id, IPC_RMID, shm_desc) == -1)
        perrorandexit("shmctl failed");
}

//-------------------------------------------------

int is_prime(int num)
{
    int i;
	for(i = 2; i*i <= num; i++)
	{
		if(num % i == 0)
			return FALSE;
	}
	return TRUE;
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
