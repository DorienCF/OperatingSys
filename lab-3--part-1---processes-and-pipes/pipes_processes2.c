// Dorien C.F @02978765 | NGTG
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int pipefd1[2], pipefd2[2];
    int pid1, pid2, pid3;
    int status;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    // create first pipe (fds go in pipefd1[0] and pipefd1[1])
    if (pipe(pipefd1) == -1) {
        perror("pipe");
        return 1;
    }

    // fork first child (P1)
    grep_args[1] = argv[1];
    pid1 = fork();

    if (pid1 == 0) {
        // P1 gets here and handles "cat scores"

        // replace standard output with output part of pipefd1
        dup2(pipefd1[1], STDOUT_FILENO);

        // close unused input half of pipefd1
        close(pipefd1[0]);

        // execute cat
        execvp("cat", cat_args);

        perror("execvp");
        return 1;
    }

    // create second pipe (fds go in pipefd2[0] and pipefd2[1])
    if (pipe(pipefd2) == -1) {
        perror("pipe");
        return 1;
    }

    // fork second child (P2)
    pid2 = fork();

    if (pid2 == 0) {
        // P2 gets here and handles "grep <argument>"

        // replace standard input with input part of pipefd1
        dup2(pipefd1[0], STDIN_FILENO);

        // close unused output half of pipefd1
        close(pipefd1[1]);

        // replace standard output with output part of pipefd2
        dup2(pipefd2[1], STDOUT_FILENO);

        // close unused input half of pipefd2
        close(pipefd2[0]);

        // execute grep
        execvp("grep", grep_args);

        perror("execvp");
        return 1;
    }

    // fork third child (P3)
    pid3 = fork();

    if (pid3 == 0) {
        // P3 gets here and handles "sort"

        // replace standard input with input part of pipefd2
        dup2(pipefd2[0], STDIN_FILENO);

        // close unused output half of pipefd2
        close(pipefd2[1]);

        // execute sort
        execvp("sort", sort_args);

        perror("execvp");
        return 1;
    }

    // parent gets here and handles nothing

    // close unused halves of pipefd1 and pipefd2
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    // wait for first two child processes to finish
    waitpid(pid1, &status, WNOHANG);
    waitpid(pid2, &status, WNOHANG);

    while (pid3 > 0) {
        // wait for third child process to exit
        waitpid(pid3, &status, 0);
        pid3 = pid3 == 0 ? -1 : pid3;
    }

    return 0;
}