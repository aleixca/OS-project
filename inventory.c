#include "inventory.h"



Product* load_inventory(char *path_2, int *total_products) {

    int fd = open(path_2, O_RDONLY);
    int i = 0;
    int n;
    Product *products = NULL;

    if (fd == -1) {
        printF("Error: Could not open inventory file.\n");
        return 0;
    }
    Product temp;
        while (read(fd, &temp, sizeof(Product)) > 0) {
        
        n = read(fd, temp.name, 100);
        temp.name[n] = '\0'; // Ensure null termination
        
        read (fd, &temp.amount, sizeof(int));
        
        read(fd, &temp.weight, sizeof(float));

        Product *new_products = realloc(products, (i + 1) * sizeof(Product));
        if (new_products == NULL) {
            printF("Error: Could not reallocate memory for products.\n");
            close(fd);
            return 0;
        }
        products = new_products;
        products[i] = temp;
        i++;
    }
    *total_products = i;
    close(fd);
    
    return products;
}

void list_products(int total_products, Product *products) {
    int i = 0;
    char *output;
    

    printF("\t\t--- Trade Ledger --- \n");
    printF("Item                 | Value (Gold) | Weight (Stone)\n");
    printF("-------------------------------------------------------\n");



    for (i = 0; i < total_products; i++) {
        char *output;
        int len = asprintf(&output, "%-20s | %10d | %12.1f\n", products[i].name, products[i].amount, products[i].weight);
        if(len == -1) {
            printF("Error creating output string\n");
            return;
        }
        printF(output);
        free(output);
    }
    printF("\n-------------------------------------------------------\n");  
    int len = asprintf(&output, "Total Products: %d\n", total_products);
    if(len == -1) {
        printF("Error creating output string\n");
        return;
    }
    printF(output);
    free(output);

}

void free_inventory(Product *products) {
    
    free(products);
    products = NULL;
}

