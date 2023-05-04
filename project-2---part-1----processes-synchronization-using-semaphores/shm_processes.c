// Dorien C.F 202978765 | GTG
#include <stdlib.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <unistd.h>

#include <sys/wait.h>

#include <string.h>

#include <errno.h>

#define SHM_NAME "/myshm"

void ClientProcess(int * );

int main(int argc, char * argv[]) {
    int ShmID;
    int * ShmPTR;
    pid_t pid;
    int status;
    key_t key;

    if (argc != 5) {
        printf("Use: %s #1 #2 #3 #4\n", argv[0]);
        exit(1);
    }

    key = ftok(".", 'x');
    if (key < 0) {
        printf("*** ftok error (server) ***\n");
        exit(1);
    }

    ShmID = shmget(key, 4 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("Server has received a shared memory of four integers...\n");

    ShmPTR = (int * ) shmat(ShmID, NULL, 0);
    if ((intptr_t) ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    ShmPTR[0] = atoi(argv[1]);
    ShmPTR[1] = atoi(argv[2]);
    ShmPTR[2] = atoi(argv[3]);
    ShmPTR[3] = atoi(argv[4]);
    printf("Server has filled %d %d %d %d in shared memory...\n",
        ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);

    printf("Server is about to fork a child process...\n");
    pid = fork();
    if (pid < 0) {
        printf("*** fork error (server) ***\n");
        exit(1);
    } else if (pid == 0) {
        ClientProcess(ShmPTR);
        exit(0);
    }

    wait( & status);
    printf("Server has detected the completion of its child...\n");
    shmdt((void * ) ShmPTR);
    printf("Server has detached its shared memory...\n");

    // Remove shared memory
    if (shmctl(ShmID, IPC_RMID, NULL) == -1) {
        printf("*** shmctl error (server) ***\n");
        exit(1);
    }
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0);
}

void ClientProcess(int * SharedMem) {
    int ShmID;
    int * ShmPTR;

    printf("   Client process started\n");

    key_t key = ftok(".", 'x');
    if (key < 0) {
        printf("*** ftok error (client) ***\n");
        exit(1);
    }

    ShmID = shmget(key, 4 * sizeof(int), 0666);
    if (ShmID < 0) {
        printf("*** shmget error (client) ***\n");
        exit(1);
    }
    printf("   Client has received a shared memory of four integers...\n");

    ShmPTR = (int * ) shmat(ShmPTR, NULL, 0);
    if ((intptr_t) ShmPTR == -1) {
        printf("*** shmat error (client) ***\n");
        exit(1);
    }
    printf(" Client has attached the shared memory...\n");
    printf(" Client found %d %d %d %d in shared memory...\n",
        ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);
    // Perform some operation on the shared memory values
    int sum = ShmPTR[0] + ShmPTR[1] + ShmPTR[2] + ShmPTR[3];
    int product = ShmPTR[0] * ShmPTR[1] * ShmPTR[2] * ShmPTR[3];

    printf("   Client has calculated the sum of %d and product of %d...\n", sum, product);

    shmdt((void * ) ShmPTR);
    printf("   Client has detached its shared memory...\n");
    printf("   Client exits...\n");
}