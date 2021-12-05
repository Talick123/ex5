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
