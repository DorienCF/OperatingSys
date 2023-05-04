#include "BENSCHILLIBOWL.h"

#include <assert.h>

#include <stdlib.h>

#include <time.h>

#include <stdbool.h>

#include <pthread.h>


// Implement the MenuItem and Order structs as described in the conversation

typedef struct MenuEntry {
        char * name;
        int price;
}
MenuItem;

typedef struct Order {
        int order_id;
        MenuItem * menu_item;
}
Order;

// Implement the BENSCHILLIBOWL struct as described in the conversation

typedef struct BENSCHILLIBOWL {
        int max_size;
        int expected_num_orders;
        Order * orders;
}
BENSCHILLIBOWL;

// Implement the BENSCHILLIBOWLMenu and BENSCHILLIBOWLMenuLength arrays as described in the conversation

MenuItem BENSCHILLIBOWLMenu[] = {
        {
                1,
                "BensChilli",
                1000
        },
        {
                2,
                "BensHalfSmoke",
                950
        },
        {
                3,
                "BensHotDog",
                850
        },
        {
                4,
                "BensChilliCheeseFries",
                800
        },
        {
                5,
                "BensShake",
                700
        },
        {
                6,
                "BensHotCakes",
                550
        },
        {
                7,
                "BensCake",
                500
        },
        {
                8,
                "BensHamburger",
                1200
        },
        {
                9,
                "BensVeggieBurger",
                1100
        },
        {
                10,
                "BensOnionRings",
                600
        },
};

int BENSCHILLIBOWLMenuLength = 10;

// Implement the PickRandomMenuItem function as described in the conversation

MenuItem * PickRandomMenuItem() {
        int random_index = rand() % BENSCHILLIBOWLMenuLength;
        MenuItem PickRandomMenuItem(BENSCHILLIBOWL * bcb);
        return random_item;
}

// Implement the OpenRestaurant function as described in the conversation

BENSCHILLIBOWL * OpenRestaurant(int max_size, int expected_num_orders) {
        BENSCHILLIBOWL * bcb = malloc(sizeof(BENSCHILLIBOWL));
        assert(bcb != NULL);

        bcb -> max_size = max_size;
        bcb -> num_orders = 0;
        bcb -> orders = NULL;
        pthread_mutex_init( & (bcb -> mutex), NULL);
        pthread_cond_init( & (bcb -> can_add_order), NULL);
        pthread_cond_init( & (bcb -> can_get_order), NULL);

        printf("Restaurant is open!\n");
        return bcb;
}

// Implement the CloseRestaurant function as described in the conversation

void CloseRestaurant(BENSCHILLIBOWL * bcb) {
        pthread_mutex_lock( & (bcb -> mutex));
        while (bcb -> num_orders > 0) {
                pthread_cond_wait( & (bcb -> can_get_order), & (bcb -> mutex));
        }
        pthread_mutex_unlock( & (bcb -> mutex));

        pthread_mutex_destroy( & (bcb -> mutex));
        pthread_cond_destroy( & (bcb -> can_add_order));
        pthread_cond_destroy( & (bcb -> can_get_order));
        free(bcb -> orders);
        free(bcb);

        printf("Restaurant is closed!\n");
}

// Implement the AddOrder function as described in the conversation

int AddOrder(BENSCHILLIBOWL * bcb, Order * order) {
        pthread_mutex_lock( & (bcb -> mutex));
        while (bcb -> num_orders == bcb -> max_size) {
                pthread_cond_wait( & (bcb -> can_add_order), & (bcb -> mutex));
        }
        Order * temp = bcb -> orders;
        if (temp == NULL) {
                bcb -> orders = order;
        } else {
                while (temp -> next != NULL) {
                        temp = temp -> next;
                }
                temp -> next = order;
        }
        bcb -> num_orders++;
        printf("Order #%d for %s has been added to the restaurant!\n", order -> order_number, order -> customer_name);
        pthread_cond_signal( & (bcb -> can_get_order));
        pthread_mutex_unlock( & (bcb -> mutex));
        return order -> order_number;
}

// Implement the GetOrder function as described in the conversation

// Implement the GetOrder function as described in the conversation

Order GetOrder(BENSCHILLIBOWL * bcb) {
        pthread_mutex_lock( & (bcb -> mutex));
        while (bcb -> num_orders == 0) {
                pthread_cond_wait( & (bcb -> can_get_order), & (bcb -> mutex));
        }
        Order * temp = bcb -> orders;
        bcb -> num_orders = bcb -> num_orders - 1;
        bcb -> orders = temp -> next;
        Order order = * temp;
        free(temp);
        printf("Order #%d for %s had been served!\n", order.order_number, order.customer_name);
        pthread_cond_signal( & (bcb -> can_add_order));
        pthread_mutex_unlock( & (bcb -> mutex));
        return order;
}