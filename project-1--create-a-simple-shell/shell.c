//Dorien C.F @02978765 | GTG
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <sys/wait.h>

#include <fcntl.h>

#include <stdbool.h>

#include <signal.h>

#define MAX_PATH 1024
#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = ">";
char delimiters[] = " \t\r\n";
extern char ** environ;
pid_t pid;
void sighandler(int);
bool Background = true;

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
    char cwd[MAX_PATH];
    char sY[100];

    // Stores the tokenized command line input.
    char * arguments[MAX_COMMAND_LINE_ARGS];

    while (true) {

        do {
            // Print the shell prompt.
            printf("%s", prompt);
            fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")

            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }

        }
        while (command_line[0] == 0x0A); // while just ENTER pressed

        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 

        // 0. Modify the prompt to print the current working directory
        printf("%s> ", getcwd(cwd, sizeof(cwd))); // prints the current working direcotry behind the prompt.
        fflush(stdout); // clears out the display buffer			  

        // 1. Tokenize the command line input (split it on whitespace)

        arguments[0] = strtok(command_line, delimiters);
        int i = 0;
        while (arguments[i] != NULL) {
            arguments[++i] = strtok(NULL, delimiters);
        }
        arguments[i] = NULL;

        // Tokenized [*]

        // 2. Implement Built-In Commands
        if (strcmp(arguments[0], "cd") == 0) {
            chdir(arguments[1]);
        } else if (strcmp(arguments[0], "exit") == 0) {
            exit(0);
        } else if (strcmp(arguments[0], "env") == 0) {
            if (arguments[1] == NULL) {
                for (i = 0; environ[i] != NULL; i++) {
                    printf("\n%s", environ[i]);
                }
                printf("\n");
            } else {
                printf("%s\n", getenv(arguments[1]));
            }
        } else if (strcmp(arguments[0], "setenv") == 0) {
            if (arguments[2] == NULL) {
                printf("Usage: setenv VAR value\n");
            } else {
                setenv(arguments[1], arguments[2], 1);
            }
        } else if (strcmp(arguments[0], "echo") == 0) {
            i = 1;

            while (arguments[i] != NULL) {

                if (strchr(arguments[i], '$') != NULL) {
                    memmove(arguments[i], arguments[i] + 1,
                        strlen(arguments[i])); 
                  // remove the '$' character from the argument
                    char * env_value = getenv(arguments[i]);
                  // get the value of the environment variable
                    if (env_value == NULL) { 
                      // if the environment variable is not set, print an empty string
                        printf(" ");
                    } else {
                        printf("%s ", env_value);
                    }
                } else {
                    printf("%s ", arguments[i]);
                }
                i++;
            }
            printf("\n");
        } else {
            // 3. Create a child process which will execute the command entered by the user.
            pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Fork Failed");
                return 1;
            } else if (pid == 0) {
                // Child process
                signal(SIGINT, sighandler);
                int fd = -1;
                if (!Background) { // if '&' not used, redirect standard error and standard output to a file
                    fd = open("output.txt", O_RDWR | O_CREAT, 0666);
                    dup2(fd, STDERR_FILENO);
                    dup2(fd, STDOUT_FILENO);
                }
                if (execvp(arguments[0], arguments) == -1) {
                    fprintf(stderr, "Unknown command\n");
                    exit(0);
                }
                if (fd != -1) close(fd);
                exit(0);
            } else {
                // Parent process
                if (!Background) { // if '&' not used, wait for child to finish
                    wait(NULL);
                } else {
                    printf("[1] %d\n", pid); // print the process ID for the background process
                }
            }
        } // Reset the arguments.
        memset(arguments, 0, MAX_COMMAND_LINE_ARGS * sizeof(char * ));
        Background = true; // reset the Background flag

    }
    return 0;
}

void sighandler(int signum) {
    printf("\nCaught signal %d, coming out...\n", signum);
    kill(pid, SIGINT); // kill the child process
    exit(1);
}