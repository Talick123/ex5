/*

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
