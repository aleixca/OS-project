#ifndef INVENTORY_H
#define INVENTORY_H

#include 'io.h'

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

typedef struct {
    char name[100];
    int amount;
    float weight;
} Product;

int total_products;

void load_inventory(char *argv[]);
void list_products(int total_products, Product *products);
void free_inventory(Product *products);


#endif
