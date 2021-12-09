/*
Noga: not suure how to end program with SIGINT ? how we can close the shared memory

    PALINDROME SERVER

    - allocates shared memory with 4 cells
    - cell 0 = getpid();
    - cell 1 = client will enter its pid
    - cell 2 - 20 = client will enter series of numbers to check palindromiut
    - cell 21 = server will enter answer to palindromiut (1 or 0)

    - server runs in an infinite loop
    - in the loop the server:
          - goes to sleep until receive SIGUSR2
          - after receiving signal, wakes, checks primenizatzia of number
          in cell in 2
          - puts answer in cell 3
          - sends SIGUSR2 to client

    - program ends when SIGINT received  (release memory also)

    ftok(".", 'q')
*/



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

// --------const and enum section------------------------

enum shm_index {PID = 0, CL_PID = 1, START_NUM = 2, RES = 21};
enum res {FALSE, TRUE};

// --------prototype section------------------------

void create_shared_mem(key_t *key, int *shm_id, int **shm_ptr);
void init_data(int *shm_ptr)
void close_shared_mem(int *shm_id, struct shmid_ds *shm_desc);

// --------main section------------------------

int main()
{
    key_t key;
    int shm_id;
    int *shm_ptr;
    struct shmid_ds shm_desc;

    signal(SIGUSR2, catch_sig2);
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
    *key = ftok(".", 'q'); // Noga: q or p ?????????????????????
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
    for(index = 1; index < ARR_SIZE; index++)
        shm_ptr[index] = 0;
}

//-------------------------------------------------

void handle_requests(int *shm_ptr)
{
    int num = 0, i;

    while(true)
    {
        pause();

        //create new array of the intigers
        for(i = 0; i < RES - START_NUM; i++)
        {
            if(shm_ptr[START_NUM + i] == 0)
                break;
            num = num * 10 + shm_ptr[START_NUM + i];
        }

        shm_ptr[RES] = is_pal(num);
        kill(shm_ptr[CL_PID], SIGUSR2);
    }
}

//-------------------------------------------------

void catch_sig2(int signum) {}

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

int is_pal(int num)
{
    int cp_num = num;
    int rev_num = 0;
    int last_dig;

    while (num > 0)
    {
        last_dig = cp_num % 10;
        rev_num = rev * 10 + last_dig;
        cp_num = cp_num / 10;
    }
    return num == rev_num;
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
