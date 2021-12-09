/*
  - via argument vector receives 1 2 or 3
  - seed(atoi(argv[1]);

  - connects to the shared memory
  - in arr[atoi(argv[1])] = 1
  - when cell 1 2 and 3 in array = 1, then go into loop
  - in a loop the program will:
      - generates number
      - if number is prime, checks if lock = 1 (0 = locked), changes it to 0,
       adds to array in shared memory
      - counts how many new primes it added and how many times
        that number that appeared the most times (same as last targil)
        (helper array)
      - changes lock = 1
      -if array is full, prints data, sends signal to array[0]


Note: when 1 child sees full, let other children see too and send signal,
  however father only needs to receive one


*/



// --------include section------------------------


#include <stdio.h>
#include <unistd.h> //for pause
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

#define ARR_SIZE 1000
#define LOCK 4



// --------const and enum section------------------------

const int ARGC_SIZE = 2;
enum STATE {LOCKED, UNLOCKED};

// --------prototype section------------------------

void check_argv(int argc);
void connect_to_shared_mem(key_t *key, int *shm_id, int **shm_ptr);
void init_and_wait(int *shm_ptr, int id);
void fill_arr(int *shm_ptr, int arr[], int *count);
void reset_arr(int arr[]);
bool prime(int num);
bool new_prime(int *shm_ptr, int curr_ind);
void print_data(int arr[], int count);
void perrorandexit(char *msg);

// --------main section------------------------

int main(int argc, char *argv[])
{
    key_t key;
    int shm_id;
    int *shm_ptr;
    int arr[ARR_SIZE], count = 0;

    check_argv(argc);
    srand(atoi(argv[1]));
    connect_to_shared_mem(&key, &shm_id, &shm_ptr);
    init_and_wait(shm_ptr, atoi(argv[1]));
    fill_arr(shm_ptr, arr, &count);
    print_data(arr, count);


    return EXIT_SUCCESS;
}

//-------------------------------------------------

void check_argv(int argc)
{
	if(argc != ARGC_SIZE)
		perrorandexit("Error! Incorrect number of arguments");
}

//-------------------------------------------------

void connect_to_shared_mem(key_t *key, int *shm_id, int **shm_ptr)
{
    *key = ftok(".", '5');
    if(*key == -1)
        perrorandexit("ftok failed");

    if((*shm_id = shmget(*key, ARR_SIZE, 0600)) == -1)
        perrorandexit("shmget failed");

    *shm_ptr = (int*)shmat(*shm_id, NULL, 0);
    if (!(*shm_ptr))
        perrorandexit("shmat failed");
}

//-------------------------------------------------

void init_and_wait(int *shm_ptr, int id)
{
    shm_ptr[id] = 1;

    while((shm_ptr[1] = 0) || (shm_ptr[2] = 0) || (shm_ptr[3] = 0))
        sleep(1);
}

//-------------------------------------------------

void fill_arr(int *shm_ptr, int arr[], int *count)
{
    int num, index = 5;

    reset_arr(arr);

    while(true)
    {
        num = rand()%(ARR_SIZE -1) + 2;

        if(prime(num))
        {
            while(shm_ptr[LOCK] == LOCKED)
                sleep(1);

            shm_ptr[LOCK] = LOCKED;

            while(shm_ptr[index] != 0)
                index++;

            if(index >= ARR_SIZE)
            {
                kill(shm_ptr[0], SIGUSR1);
                shm_ptr[LOCK] = 1; //gives other processes chance to see its full
                return;
            }

            shm_ptr[index] = num;
            arr[num]++;

            if(new_prime(shm_ptr, index))
                (*count)++;

            shm_ptr[LOCK] = UNLOCKED;
        }
    }
}

//-------------------------------------------------

void reset_arr(int arr[])
{
	int index;
	for(index = 0; index < ARR_SIZE; index++)
		arr[index] = 0;
}

//-------------------------------------------------

bool prime(int num)
{
	int i;
	for(i = 2; i*i <= num; i++)
		if(num % i == 0)
			return false;

	return true;
}

//-------------------------------------------------

bool new_prime(int *shm_ptr, int curr_ind)
{
    int index;

    for(index = 5; index < curr_ind; index++)
        if(shm_ptr[index] == shm_ptr[curr_ind])
            return false;

    return true;
}


void print_data(int arr[], int count)
{
    int max_count = 0, prime, index;

    for(index = 0; index < 0; index++)
        if(arr[index] > max_count)
            prime = index;

    printf("Process %d sent % different new primes. The prime it sent most was %d, %d times ",
    	   (int)getpid(), count, prime, max_count);
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
