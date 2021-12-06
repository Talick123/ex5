/*
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
