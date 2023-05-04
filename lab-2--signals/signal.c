// Dorien C.F @02978765 | MGTG
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>


// Signal handler function to print "Hello World!" and "Turing was right!" messages
void my_handler(int signal_number) {
    printf("Hello World!\n");
    printf("Turing was right!\n");
}

// Signal handler function to print the total time and exit the program
void my_ctrlc_handler(int signal_number) {
    printf("\nTotal time: %d seconds\n", alarm(0));
    exit(0);
}

int main() {
    // Register signal handler functions for SIGALRM and SIGINT signals
    signal(SIGALRM, my_handler);
    signal(SIGINT, my_ctrlc_handler);
    
    while (1) {
        // Schedule a 1 second SIGALRM and wait for it to be delivered
        alarm(1);
        pause();
    }
    
    return 0;
}
