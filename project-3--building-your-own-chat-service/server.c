#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdbool.h>

#include <pthread.h>

#include <signal.h>

#include "server.h"

#define MAX_ROOM_NAME 50
#define MAX_MSG_LEN 512
#define PORT 8888
#define BACKLOG 10
#define SHUTDOWN_TIME 3

int chat_serv_sock_fd; // server socket

typedef struct User {
   int socket;
   char name[MAX_ROOM_NAME];
   struct User * next;
}
User;

typedef struct UserListNode {
   User * user;
   struct UserListNode * next;
}
UserListNode;

typedef struct UserList {
   int num_users;
   UserListNode * head;
}
UserList;

typedef struct Room {
   char name[MAX_ROOM_NAME];
   int num_users;
   UserList * head;
}
Room;

typedef struct RoomNode {
   Room room;
   struct RoomNode * next;
}
RoomNode;

RoomNode * create_room_node(Room * room) {
   RoomNode * node = malloc(sizeof(RoomNode));
   node -> room = * room;
   node -> next = NULL;
   return node;
}

void destroy_user_list(UserList * user_list) {
   UserListNode * curr_user = user_list -> head;
   while (curr_user != NULL) {
      UserListNode * next_user = curr_user -> next;
      free(curr_user -> user);
      free(curr_user);
      curr_user = next_user;
   }
}

void destroy_room_list(RoomNode ** room_list_ptr) {
   RoomNode * curr_room = * room_list_ptr;
   while (curr_room != NULL) {
      RoomNode * next_room = curr_room -> next;
      destroy_user_list(curr_room -> room.head);
      free(curr_room);
      curr_room = next_room;
   }
   * room_list_ptr = NULL;
}

/////////////////////////////////////////////
// USE THESE LOCKS AND COUNTER TO SYNCHRONIZE

int numReaders = 0; // keep count of the number of readers

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex lock
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER; // read/write lock
pthread_rwlock_t rwlock;
RoomNode * room_list;

/////////////////////////////////////////////

char
const * server_MOTD = "Thanks for connecting to the BisonChat Server.\n\nchat>";

void sigintHandler(int sig_num) {
   // Send message to all connected clients that the server is shutting down
   char shutdown_msg[MAX_MSG_LEN];
   sprintf(shutdown_msg, "\nServer is going down in %d seconds. Goodbye!\n", SHUTDOWN_TIME);

   pthread_rwlock_wrlock( & rw_lock);

   void sigintHandler(int sig_num) {
      // Send message to all connected clients that the server is shutting down
      char shutdown_msg[MAX_MSG_LEN];
      sprintf(shutdown_msg, "\nServer is going down in %d seconds. Goodbye!\n", SHUTDOWN_TIME);
      pthread_rwlock_wrlock( & rwlock); // acquire write lock on room list
      RoomNode * curr_room = room_list;
      while (curr_room != NULL) {
         UserListNode * curr_user = curr_room -> room.head -> head;
         while (curr_user != NULL) {
            send(curr_user -> user -> socket, shutdown_msg, strlen(shutdown_msg), 0);
            curr_user = curr_user -> next;
         }
         curr_room = curr_room -> next;
      }
      pthread_rwlock_unlock( & rwlock); // release write lock on room list

      // Wait for SHUTDOWN_TIME seconds before shutting down the server
      sleep(SHUTDOWN_TIME);

      // Destroy room list and exit
      destroy_room_list( & room_list);
      close(chat_serv_sock_fd);
      exit(0);
   }
}
void add_user_to_room(Room * room, User * user) {
   UserListNode * new_user_node = malloc(sizeof(UserListNode));
   new_user_node -> user = user;
   new_user_node -> next = room -> head -> head;
   room -> head -> head = new_user_node;
   room -> num_users++;
}

void remove_user_from_room(Room * room, User * user) {
   UserListNode * curr_user = room -> head -> head;
   UserListNode * prev_user = NULL;
   while (curr_user != NULL) {
      if (curr_user -> user == user) {
         if (prev_user == NULL) {
            room -> head -> head = curr_user -> next;
         } else {
            prev_user -> next = curr_user -> next;
         }
         free(curr_user);
         room -> num_users--;
         return;
      }
      prev_user = curr_user;
      curr_user = curr_user -> next;
   }
}

void handle_client_message(User * user, char * msg) {
   pthread_rwlock_rdlock( & rwlock); // acquire read lock on room list
   RoomNode * curr_room = room_list;
   while (curr_room != NULL) {
      UserListNode * curr_user = curr_room -> room.head -> head;
      while (curr_user != NULL) {
         if (curr_user -> user != user) {
            send(curr_user -> user -> socket, msg, strlen(msg), 0);
         }
         curr_user = curr_user -> next;
      }
      curr_room = curr_room -> next;
   }
   pthread_rwlock_unlock( & rwlock); // release read lock on room list
}

void * client_handler(void * arg) {
   User * user = (User * ) arg;
   // Send server message of the day
   send(user -> socket, server_MOTD, strlen(server_MOTD), 0);

   char msg[MAX_MSG_LEN];
   int read_size;
   while ((read_size = recv(user -> socket, msg, MAX_MSG_LEN, 0)) > 0) {
      msg[read_size] = '\0';

      // Handle client message
      handle_client_message(user, msg);
   }

   // Client disconnected
   pthread_rwlock_wrlock( & rwlock); // acquire write lock on room list
   RoomNode * curr_room = room_list;
   while (curr_room != NULL) {
      if (strcmp(user -> name, curr_room -> room.name) == 0) {
         remove_user_from_room( & curr_room -> room, user);
         if (curr_room -> room.num_users == 0) {
            // If no users

            void sigintHandler(int sig_num) {
               // Send message to all connected clients that the server is shutting down
               char shutdown_msg[MAX_MSG_LEN];
               sprintf(shutdown_msg, "\nServer is going down in %d seconds. Goodbye!\n", SHUTDOWN_TIME);
               pthread_rwlock_wrlock( & rw_lock);
               RoomNode * curr_room = room_list;
               while (curr_room != NULL) {
                  UserListNode * curr_user = curr_room -> room.head -> head;
                  while (curr_user != NULL) {
                     write(curr_user -> user -> socket, shutdown_msg, strlen(shutdown_msg));
                     curr_user = curr_user -> next;
                  }
                  curr_room = curr_room -> next;
               }
               pthread_rwlock_unlock( & rw_lock);

               // Wait for SHUTDOWN_TIME seconds to give clients time to receive message
               sleep(SHUTDOWN_TIME);

               // Destroy all data structures
               pthread_rwlock_wrlock( & rw_lock);
               destroy_room_list( & room_list);
               pthread_rwlock_unlock( & rw_lock);

               // Close server socket
               close(chat_serv_sock_fd);

               exit(0);
            }

            void init() {
               // Initialize room list
               room_list = NULL;
               // Initialize read-write lock
               if (pthread_rwlock_init( & rwlock, NULL) != 0) {
                  perror("pthread_rwlock_init() failed");
                  exit(1);
               }

               // Register signal handler for SIGINT
               if (signal(SIGINT, sigintHandler) == SIG_ERR) {
                  perror("signal() failed");
                  exit(1);
               }

               // Create server socket
               chat_serv_sock_fd = create_server_socket(PORT, BACKLOG);

               printf("Server listening on port %d...\n", PORT);

               while (true) {
                  int client_sock_fd = accept_connection(chat_serv_sock_fd);
                  pthread_t client_thread;
                  pthread_create( & client_thread, NULL, client_handler, (void * ) & client_sock_fd);
               }
            }

            int main() {
               init();
            }
         }
         pthread_rwlock_unlock( & rwlock); // release write lock on room list
         return;
      }
   }
   return 0;
}