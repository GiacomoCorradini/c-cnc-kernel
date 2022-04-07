// Linked list

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structure representing an object in the list
typedef struct {
  char *id;
  // double value;
  void *next;
  void *prev;
} element_t;

element_t *element_new(char *id) {
  element_t *e = malloc(sizeof(element_t));
  memset(e, 0, sizeof(element_t));
  e->id = malloc(strlen(id) + 1);
  strncpy(e->id, id, strlen(id));
  return e;
}

void element_free(element_t *e) {
  free(e->id);
  free(e);
}

// structure representing the list itself
typedef struct {
  element_t *first, *last;
  size_t length;
} list_t;

// create a new list
// @param the id of the first element
// @return a list
list_t *list_new(char *id) {
  // allocate the memory for the list
  list_t *l = malloc(sizeof(list_t));
  memset(l, 0,
         sizeof(list_t)); // set the byte of 'l' to '0' (optional but good rule)
  // create a new element
  element_t *e = element_new(id);
  // initialize list element
  l->first = e;
  l->last = e;
  l->length = 1;
  return l;
}

// append an existing element in an existing list
void list_append_element(list_t *list, element_t *e) {
  list->last->next = e; // the next element of current last becomes e
  e->prev = list->last; // the previous element of e becomes the previous last
  list->last = e;       // the new last element is e
  e->next = NULL;       // there is nothing after e
  list->length++;       // increase the length by one
}

// creating and append a new element
element_t *list_append(list_t *list, char *id) {
  element_t *e = element_new(id);
  list_append_element(list, e);
  return e;
}

// insert a new element after a given ID
void list_insert_element(list_t *list, element_t *new, char *after) {
  element_t *e;
  e = list->first;
  do {
    if (strcmp(e->id, after) == 0) { // use strcmp to compare to string
      new->prev = e;
      new->next = e->next;
      ((element_t *)e->next)->prev = new; // cast
      e->next = new;
      list->length++;
      break;
    }
  } while ((e = e->next)); // equal to write (e = e->next) != NULL
}
// creating and insert a new element after a given ID
element_t *list_insert(list_t *list, char *id, char *after) {
  element_t *e = element_new(id);
  list_insert_element(list, e, after);
  return e;
}

// remove an element
void list_delete(list_t *list, char *id) {
  element_t *e;
  e = list->first;
  do {
    if (strcmp(e->id, id) == 0) {
      ((element_t *)e->prev)->next = e->next;
      ((element_t *)e->next)->prev = e->prev;
      element_free(e);
      list->length--;
      break;
    }
  } while ((e = e->next));
}

// free the memory
void list_free(list_t *list) {
  element_t *e, *next;
  e = list->first;
  do {
    next = e->next;
    element_free(e);
  } while ((e = next));
  free(list);
}

typedef enum { ASC, DESC } loop_order_t;
typedef void (*loop_fun_t)(element_t *e, loop_order_t o, void *userdata);

// perform a given operation on all the element of the list
void list_loop(list_t *list, loop_fun_t fun, loop_order_t order,
               void *userdata) {
  element_t *e;
  if (order == ASC)
    e = list->first;
  else
    e = list->last;

  do {
    fun(e, order, userdata);
    if (order == ASC)
      e = e->next;
    else
      e = e->prev;
  } while (e);
}

void print_element(element_t *e, loop_order_t o, void *userdata) {
  printf("%10s: %15p -> %15p -> %15p\n", e->id, e->prev, e, e->next);
}

void print_element_with_index(element_t *e, loop_order_t o, void *userdata){
  size_t *i = (size_t*)userdata;
  printf("list[%lu]: %s\n", *i, e->id);
  if(o == ASC){
    (*i)++;
  } else {
    (*i)--;
  }
}

int main() {
  char id[4][10] = {"one", "two", "three", "four"};
  element_t *e;
  size_t i;

  // create a list
  list_t *list = list_new("zero");

  // populate the list with other element
  for (i = 0; i < 4; i++) {
    // create an element and append it to list
    e = element_new(id[i]);
    list_append_element(list, e);
    // or alternatively
    // list_append(list, id[i]);
  }

  list_loop(list, print_element, ASC, NULL);

  list_insert(list, "two.five","two");

  list_loop(list, print_element, DESC, NULL);

  i = 0;
  list_loop(list, print_element_with_index, ASC, &i);

  i = list->length - 1;
  list_loop(list, print_element_with_index, DESC, &i);

  list_free(list);

  return 0;
}