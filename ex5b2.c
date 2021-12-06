/*
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
