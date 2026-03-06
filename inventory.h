#ifndef INVENTORY_H
#define INVENTORY_H
#define _GNU_SOURCE

#include "io.h"
#include "maester.h"


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


typedef struct {
    char name[100];
    int amount;
    float weight;
} Product;

Product* load_inventory(char *path_2,  int *total_products);
void list_products(int total_products, Product *products);
void free_inventory(Product *products);


#endif
