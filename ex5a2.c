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
#define START 5

// --------const and enum section------------------------

const int ARGC_SIZE = 2;
enum STATE {LOCKED, UNLOCKED};

// --------prototype section------------------------

void check_argv(int argc);
void connect_to_shared_mem(int **shm_ptr);
void init_and_wait(int *shm_ptr, int id);
void fill_arr(int *shm_ptr);
bool prime(int num);
bool new_prime(int *shm_ptr, int curr_ind);
int count_appearances(int *shm_ptr,int curr_ind);
void print_data(int new_count,int max,int max_prime);
void perrorandexit(char *msg);

// --------main section------------------------

int main(int argc, char *argv[])
{
    int *shm_ptr;

    check_argv(argc);
    srand(atoi(argv[1]));
    connect_to_shared_mem(&shm_ptr);
    init_and_wait(shm_ptr, atoi(argv[1]));
    fill_arr(shm_ptr);

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void check_argv(int argc)
{
	if(argc != ARGC_SIZE)
		perrorandexit("Error! Incorrect number of arguments");
}

//-------------------------------------------------

void connect_to_shared_mem(int **shm_ptr)
{
    key_t key;
    int shm_id;

    key = ftok(".", '5');
    if(key == -1)
        perrorandexit("ftok failed");

    if((shm_id = shmget(key, ARR_SIZE, 0600)) == -1)
        perrorandexit("shmget failed");

    *shm_ptr = (int*)shmat(shm_id, NULL, 0);
    if (!(*shm_ptr))
        perrorandexit("shmat failed");

    printf("Shm_id: %d\n", shm_id);
}

//-------------------------------------------------

void init_and_wait(int *shm_ptr, int id)
{
    shm_ptr[id] = 1;


    while((shm_ptr[1] == 0) || (shm_ptr[2] == 0) || (shm_ptr[3] == 0))
        sleep(1);
}

//-------------------------------------------------

void fill_arr(int *shm_ptr)
{
    int num, index = START, max, max_prime = 0, new_count, other;
    max = new_count = 0;

    while(true)
    {
        num = (rand() + 2);

        if(prime(num))
        {
			if(max_prime == 0)
			{
				max_prime = num;
				max++;
			}

            while(shm_ptr[LOCK] == LOCKED)
                sleep(1);

            shm_ptr[LOCK] = LOCKED;

            while(shm_ptr[index] != 0 && index < ARR_SIZE)
                index++;

            if(index >= ARR_SIZE)
            {
                kill(shm_ptr[0], SIGUSR1);
                shm_ptr[LOCK] = UNLOCKED; //gives other processes chance to see its full
                print_data(new_count, max, max_prime);
                return;
            }

            shm_ptr[index] = num;

            if(new_prime(shm_ptr, index))
                new_count++;
            else
            {
				other = count_appearances(shm_ptr, index);
				if(max < other)
				{
					max = other + 1;
					max_prime = num;
				}
            }

            shm_ptr[LOCK] = UNLOCKED;
        }
    }
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

    for(index = START; index < curr_ind; index++)
        if(shm_ptr[index] == shm_ptr[curr_ind])
            return false;

    return true;
}

//-------------------------------------------------

int count_appearances(int *shm_ptr,int curr_ind)
{
    int counter = 0, index;

    for(index = 0; index < curr_ind; index++)
        if(shm_ptr[index] == shm_ptr[curr_ind])
            counter++;

    return counter;
}

//-------------------------------------------------

void print_data(int new_count,int max,int max_prime)
{
    printf("Process %d sent %d different new primes.",
        (int)getpid(), new_count);

     printf("The prime it sent most was %d, %d times \n",
        	   max_prime, max);
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
