#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS *ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
// define a function to simulate the behavior of a customer
void *BENSCHILLIBOWLCustomer(void *tid) {
  // get the customer's ID from the thread ID
  int customer_id = (int)(long)tid;

  // create a variable to store the customer's order
  Order *my_order;

  // loop for the number of orders each customer makes
  for (i = 0; i < ORDERS_PER_CUSTOMER; i++) {
    // allocate memory for the new order
    my_order = malloc(sizeof(Order));

    // initialize the next pointer to NULL
    my_order->next = NULL;

    // choose a random menu item for the order
    my_order->menu_item = PickRandomMenuItem();

    // set the customer ID for the order
    my_order->customer_id = customer_id;

    // add the order to the restaurant
    AddOrder(bcb, my_order);

    // print a message indicating that the customer has requested an order
    printf("Customer %d requested order #%d: %s\n", my_order->customer_id,
           my_order->order_number, my_order->menu_item);
  }

  // return NULL to indicate that the thread has completed
  return NULL;
}


/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void *BENSCHILLIBOWLCook(void *tid) {
  int cook_id = (int)(long)tid;
  // int orders_fulfilled = 0;
  Order *current_order;
  while (bcb->current_size > 0) {
    current_order = GetOrder(bcb);
    // printf("Cook #%d fulfilled %d orders\n", cook_id, ++orders_fulfilled);
    printf("Cook #%d fulfilled order #%d, %s for customer %d\n", cook_id,
           current_order->order_number, current_order->menu_item,
           current_order->customer_id);
  }
  return NULL;
}

/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
  srandom(time(NULL));
  int i, j;
  int cook_ids[NUM_COOKS];
  int customer_ids[NUM_CUSTOMERS];

  bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);
  pthread_t cook_threads[NUM_COOKS];
  pthread_t customer_threads[NUM_CUSTOMERS];

  // create threads for each customer
for (i = 0; i < NUM_CUSTOMERS; i++) {
  // assign each customer a unique ID
  customer_ids[i] = i + 1;
  // create a thread for the customer and pass in the customer's ID
  pthread_create(&customer_threads[i], NULL, BENSCHILLIBOWLCustomer,
                 &(customer_ids[i]));
}

// create threads for each cook
for (i = 0; i < NUM_COOKS; i++) {
  // assign each cook a unique ID
  cook_ids[i] = i + 1;
  // create a thread for the cook and pass in the cook's ID
  pthread_create(&cook_threads[i], NULL, BENSCHILLIBOWLCook, &(cook_ids[i]));
}

// wait for all customer threads to complete
for (j = 0; j < NUM_CUSTOMERS; j++) {
  pthread_join(customer_threads[j], NULL);
}

// wait for all cook threads to complete
for (j = 0; j < NUM_COOKS; j++) {
  pthread_join(cook_threads[j], NULL);
}

// close the restaurant
CloseRestaurant(bcb);

// return 0 to indicate success
return 0;

}
