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
