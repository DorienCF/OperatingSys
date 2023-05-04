#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"


int main() {
  printf("Tests for linked list implementation\n");

  list_t *list = list_alloc();
  

  list_print(list); // should print out "list is empty"..

  list_free(list); // Should print out "The list is free"..

  list_add_to_back(list, 0);
  list_add_to_back(list, 1);
  list_add_to_back(list, 2);
  list_add_to_back(list, 3);
  list_add_to_back(list, 4);
  list_add_to_back(list, 5);
   // Should creat a new list with the following elements: [5,4,3,2,1,0]
  list_print(list);
  printf("\n");
  
  list_add_to_front(list, 6);
  list_add_to_front(list, 7);
  list_add_to_front(list, 8);
    // Should create a new list with the following elements: [8,7,6,5,4,3,2,1,0]
  list_print(list); // Will print the following. 
  
  printf("\n");  
  return 0;

}
