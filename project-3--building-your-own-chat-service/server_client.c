#include "server.h"

#define DEFAULT_ROOM "Lobby"

// USE THESE LOCKS AND COUNTER TO SYNCHRONIZE
extern int numReaders;
extern pthread_mutex_t rw_lock;
extern pthread_mutex_t mutex;

extern struct node * head;

extern
const char * server_MOTD;

/*
 *Main thread for each client.  Receives all messages
 *and passes the data off to the correct function.  Receives
 *a pointer to the file descriptor for the socket the thread
 *should listen on
 */

char * trimwhitespace(char * str) {
        char * end;

        // Trim leading space
        while (isspace((unsigned char) * str))
                str++;

        if ( * str == 0) // All spaces?
                return str;

        // Trim trailing space
        end = str + strlen(str) - 1;
        while (end > str && isspace((unsigned char) * end))
                end--;

        // Write new null terminator character
        end[1] = '\0';

        return str;
}

void * client_receive(void * ptr) {
        int client = * (int * ) ptr; // socket

        int received, i;
        char buffer[MAXBUFF], sbuffer[MAXBUFF]; //data buffer of 2K
        char tmpbuf[MAXBUFF]; //data temp buffer of 1K
        char cmd[MAXBUFF], username[20];
        char * arguments[80];

        struct node * currentUser;

        send(client, server_MOTD, strlen(server_MOTD), 0); // Send Welcome Message of the Day.

        // Creating the guest user name
        sprintf(username, "guest%d", client);
        head = insertFirstU(head, client, username);

        // Add the GUEST to the DEFAULT ROOM (i.e. Lobby)

        while (1) {
                if ((received = read(client, buffer, MAXBUFF)) != 0) {
                        buffer[received] = '\0';
                        strcpy(cmd, buffer);
                        strcpy(sbuffer, buffer);

                        /////////////////////////////////////////////////////
                        // we got some data from a client

                        // 1. Tokenize the input in buf (split it on whitespace)

                        // get the first token

                        arguments[0] = strtok(cmd, delimiters);

                        // walk through other tokens

                        i = 0;
                        while (arguments[i] != NULL) {
                                arguments[++i] = strtok(NULL, delimiters);
                                strcpy(arguments[i - 1], trimwhitespace(arguments[i - 1]));
                        }

                        // Arg[0] = command
                        // Arg[1] = user or room

                        /////////////////////////////////////////////////////
                        // 2. Execute command: TODO

                        if (strcmp(arguments[0], "create") == 0) {
                                printf("create room: %s\n", arguments[1]);

                                // perform the operations to create room arg[1]

                                sprintf(buffer, "\nchat>");
                                send(client, buffer, strlen(buffer), 0); // send back to client
                        } else if (strcmp(arguments[0], "join") == 0) {
                                printf("join room: %s\n", arguments[1]);

                                // perform the operations to join room arg[1]

                                sprintf(buffer, "\nchat>");
                                send(client, buffer, strlen(buffer), 0); // send back to client
                        } else if (strcmp(arguments[0], "leave") == 0) {
                                printf("leave room: %s\n", arguments[1]);

                                // perform the operations to leave room arg[1]

                                sprintf(buffer, "\nchat>");
                                send(client, buffer, strlen(buffer), 0); // send back to client
                        } else if (strcmp(arguments[0], "list") == 0) {
                                printf("list rooms\n");
                                // perform the operations to list rooms

                                sprintf(buffer, "\nchat>");
                                send(client, buffer, strlen(buffer), 0); // send back to client
                        } else if (strcmp(arguments[0], "users") == 0) {
                                printf("list users in room: %s\n", arguments[1]);

                                // perform the operations to list users in room arg[1]

                                sprintf(buffer, "\nchat>");
                                send(client, buffer, strlen(buffer), 0); // send back to client
                        } else if (strcmp(arguments[0], "quit") == 0) {
                                printf("User %s has disconnected\n", username);

                                // perform the operations to remove user from chat

                                pthread_mutex_lock( & rw_lock);
                                head = deleteUser(head, username);
                                pthread_mutex_unlock( & rw_lock);

                                sprintf(buffer, "Goodbye!\n");
                                send(client, buffer, strlen(buffer), 0); // send back to client
                                close(client);
                                return NULL;
                        } else {
                                sprintf(buffer, "Invalid command\n");
                                send(client, buffer, strlen(buffer), 0); // send back to client
                        }
                }
        }
}

int main(int argc, char ** argv) {
        int sockfd, newsockfd, portno, clilen;
        struct sockaddr_in serv_addr, cli_addr;
        pthread_t thread_id;
        if (argc < 2) {
                fprintf(stderr, "ERROR, no port provided\n");
                exit(1);
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");

        bzero((char * ) & serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

        if (bind(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR on binding");

        listen(sockfd, 5);
        clilen = sizeof(cli_addr);

        printf("Server running on port %d...\n", portno);

        while (1) {
                newsockfd = accept(sockfd, (struct sockaddr * ) & cli_addr, & clilen);

                if (newsockfd < 0)
                        error("ERROR on accept");

                printf("New client connected: %d\n", newsockfd);

                // Creating new thread for the client
                pthread_create( & thread_id, NULL, & client_receive, & newsockfd);
        }

        return 0;
}