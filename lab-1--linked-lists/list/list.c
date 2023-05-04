// list/list.c
//
// Implementation for linked list.
//
// <Author>

//  
// 
// Implementation for linked list.
//
// 

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include "list.h"

list_t * list_alloc() {
        list_t * list = (list_t * ) malloc(sizeof(list_t));
        list -> head = NULL;
        return list;
}

void list_free(list_t * l) {

        int i;
        int count = list_length(l);

        for (i = 0; i < count; i++) {
                list_remove_from_front(l);
        }

        if (l -> head == NULL) {
                printf("The list is free!\n");
        }

}

void list_print(list_t * l) {

        node_t * curr_node;
        curr_node = l -> head;

        if (curr_node == NULL) {
                printf("list is empty\n");
        }

        while (curr_node != NULL) {
                printf("%d -> ", curr_node -> value);
                curr_node = curr_node -> next;
        }

}

int list_length(list_t * l) {

        node_t * curr_node = l -> head;
        int length = 0;
        while (curr_node != NULL) { // the while loop will iterate through the linked list until it is NULL.
                length++;
                curr_node = curr_node -> next;
        }
        return length;

}

void list_add_to_back(list_t * l, elem value) {
        list_add_at_index(l, value, list_length(l));
}

void list_add_to_front(list_t * l, elem value) {

        list_add_at_index(l, value, 0);

}

void list_add_at_index(list_t * l, elem value, int index) // Needs to be tested 
{
        int size = list_length(l); // Determining the lenqgth of the list.
        node_t * new_node = (node_t * ) malloc(sizeof(node_t)); // Allocating memory for a new node.
        new_node -> value = value; // Setting the value of the new node.
        new_node -> next = NULL; // Setting the next pointer to NULL.

        if (index == 0) {
                new_node -> next = l -> head;
                l -> head = new_node;
        }
        // empty list, out of range
        else if ((index < 0 && l -> head == NULL) || (index > size && l -> head == NULL)) {
                new_node -> next = l -> head;
                l -> head = new_node;
        }
        // nonempty list, out of range
        else if (index > size && l -> head != NULL) {
                node_t * tail = l -> head;
                while (tail -> next != NULL) {
                        tail = tail -> next;
                }
                tail -> next = new_node;
        } else {
                node_t * curr = l -> head;
                while (--index) {
                        curr = curr -> next;
                }
                new_node -> next = curr -> next;
                curr -> next = new_node;
        }

}

elem list_remove_from_back(list_t * l) {

        node_t * curr = l -> head;
        if (curr == NULL) {
                return -1;
        }

        while (curr -> next -> next != NULL) {
                curr = curr -> next;
        }

        free(curr -> next);
        curr -> next = NULL;
        return 0;

}

elem list_remove_from_front(list_t * l) {

        node_t * temp = (node_t * ) malloc(sizeof(node_t));
        temp = l -> head;
        l -> head = l -> head -> next;
        free(temp);
        return 0;
}
elem list_remove_at_index(list_t * l, int index)

{
        node_t * curr = l -> head;
        node_t * temp = (node_t * ) malloc(sizeof(node_t));
        int count = 0;
        if (curr == NULL) {
                return 0;
        }

        while (count < index - 1) {
                curr = curr -> next;
                count++;
        }

        temp = curr -> next;
        curr -> next = curr -> next -> next;
        free(temp);
        return 0;
}

bool list_is_in(list_t * l, elem value)

{
        int x;
        node_t * curr = l -> head;
        int count = list_length(l);

        for (x = 0; x < count; x++) {
                if (curr -> value == x) {
                        return true;
                }
                curr = curr -> next;
        }

        return false;
}

elem list_get_elem_at(list_t * l, int index)

{
        int i = 0;
        int len;
        node_t * curr = l -> head;
        int count = index;

        len = list_length(l);
        if (index > len) {
                return -1;
        }

        if (index == 0) {
                return curr -> value;
        }

        while (i < count) {
                curr = curr -> next;
                i++;
        }

        return curr -> value;
}

int list_get_index_of(list_t * l, elem value)

{
        return -1;
}