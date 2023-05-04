// Dorien C.F @02978765 | MGTG
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

bool signaled = true;
int num_alarms = 0;
time_t start, ending;

void handler(int signal_number) {
    printf("Turing was right!\n");
    num_alarms++;
    signaled = false;
}

void ctrlc_handler(int signal_number) {
    time(&ending);
    double elapsed = difftime(ending, start);
    printf("\nNumber of alarms: %d, Total time: %fs\n", num_alarms, elapsed);
    exit(0);
}

int main() {
    signal(SIGALRM, handler);
    signal(SIGINT, ctrlc_handler);
    alarm(1);
    time(&start);
    while (1) {
        signaled = true;
        while (signaled);
    }
    return 0;
}
