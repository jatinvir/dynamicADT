#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "pool.h"
#include <string.h>
struct poolnode {
  char *begin;
  char *end;
  struct poolnode *next;
  struct poolnode *prev;
  int spaces;
};

struct pool {
  char *data;
  int available;
  int distance; 
  struct poolnode *root;
};


struct pool *pool_create(int size) {
  assert(size >= 0);
  struct pool *p = malloc(sizeof(struct pool));
  p->data = malloc(size * sizeof(char));
  p->available = size;
  p->distance = size;
  p->root = NULL;
  return p;
}



bool pool_destroy(struct pool *p) {
  assert(p);
  if (!p->root) {
    free(p->data);
    free(p);
    return true;
  } else {
    return false;
  }
}

char *root_null(struct pool *p, struct poolnode *newnode, int size) {
      p->available -= size;
      newnode->begin = p->data;
      newnode->end = newnode->begin + (size - 1);
      newnode->next = NULL;
      newnode->prev = NULL;
      newnode->spaces = p->available;
      p->root = newnode;
      return newnode->begin;
}

char *distance_size(struct pool *p, struct poolnode *newnode, int size) {
      p->root->prev = newnode;
      p->root = newnode;
      newnode->begin = p->data + 1;
      newnode->next = p->root;
      newnode->prev = NULL;
      newnode->end = newnode->begin + size;
      newnode->spaces = p->distance;
      p->available -= size;
      return newnode->begin;
}

char *pool_alloc(struct pool *p, int size) {
  assert(p);
  assert(size > 0);
  if (size > p->available) {
    return NULL;
  }
  else {
    struct poolnode *newnode = malloc(sizeof(struct poolnode));
    if (!p->root || size <= p->distance) {
      p->distance = 0;
    }
    if (p->root == NULL) {
      return root_null(p, newnode, size);
    }
    else if (p->distance >= size) {
      return distance_size(p, newnode, size);
     
    }
    else {
      struct poolnode *temp = p->root;
      while (p->root) {
        if (temp->spaces >= size) {
          newnode->begin = temp->end + 1;
          newnode->end = size - 1 + newnode->begin;
          newnode->spaces = temp->spaces - size;
          newnode->next = temp->next;
          temp->next = newnode;
          p->available -= size;
          temp->spaces = 0;
          if (newnode->next) {
            newnode->next->prev = newnode;
          }
          return newnode->begin;
        } else {
            temp = temp->next;
        }
      }
      return NULL;
    }
  }
}


bool pool_free(struct pool *p, char *addr) {
  assert(addr);
  assert(p);
  struct poolnode *temp = p->root;
  
  while(temp) {
    if (temp->begin == addr) {
      int space = temp->end - temp->begin + 1;
      p->available += space;
      if (!temp->prev) {
        p->root = temp->next;
        p->distance = p->distance + space + temp->spaces;
      }
      if (temp->next != NULL) {
        temp->next->prev = temp->prev;
      }
      free(temp);
      return true;
    }
    temp = temp->next;
  }
  free(temp);
  return false;
}

//char *depend_size(struct poolnode *temp, 


char *pool_realloc(struct pool *p, char *addr, int size) {
  assert(p);
  assert(addr);
  assert(size > 0);
  struct poolnode *temp = p->root;
  while (temp != NULL) {
    if (temp->begin == addr) {
      temp->end += 1;
      int space = temp->end - temp->begin;
      if (size <= space || (space + temp->spaces) >= size) {
        temp->end = temp->begin + size - 1;
        temp->spaces += space - size;
        p->available += space - size;
        return temp->begin;
      }
    } 
    temp = temp->next;
  }
  return NULL;
}



void pool_print_active(const struct pool *p) {
  assert(p);
  if (p->root) {
    int index = 0;
    int size = 0;
    int space = p->root->end - p->root->begin + 1;
    int pos = p->root->begin - p->data;
    printf("active: %d [%d]", pos, space);
    struct poolnode *temp = p->root->next;
    while (temp) {
   index = temp->begin - p->data;
       size = temp->end - temp->begin + 1;
      printf(", %d [%d]", index, size);
      temp = temp->next;
    }
    printf("\n");
  } else {
  printf("active: none\n");
  }
}




void pool_print_available(const struct pool *p) {
  assert(p);
  if (p->available == 0) {
    printf("available: none\n");
  }
  else {
    if (p->distance != 0) {
      printf("available: 0 [%d]", p->distance);
      struct poolnode *root = p->root;
      while (root) {
        int index = root->end + 1 - p->data;
        printf(", %d [%d]", index, root->spaces);
        root = root->next;
      }
    }
    else {
      int index = p->root->end + 1 - p->data;
      printf("available: %d [%d]", index, p->root->spaces);
      struct poolnode *root_next = p->root->next;
      while (root_next) {
        int indexes = root_next->end + 1 - p->data;
        printf(", %d [%d]", indexes, root_next->spaces);
        root_next = root_next->next;
      }
    }
   printf("\n");
  }
}