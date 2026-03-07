#include "inventory.h"


Product* load_inventory(char *path_2, int *total_products) {
    int fd = open(path_2, O_RDONLY);
    int i = 0;
    Product *products = NULL;
    Product temp;

    if (fd == -1) {
        printF("Error: Could not open inventory file.\n");
        return NULL;
    }

    while (read(fd, &temp, sizeof(Product)) != 0) {
        Product *new_products = realloc(products, (i + 1) * sizeof(Product));
        if (new_products == NULL) {
            printF("Error: Could not reallocate memory for products.\n");
            free(products);
            close(fd);
            return NULL;
        }

        products = new_products;
        products[i] = temp;
        i++;
    }

    close(fd);
    *total_products = i;
    return products;
}

void list_products(int total_products, Product *products) {
    int i = 0;
    char *output;
    

    printF("--- Trade Ledger --- \n");
    char *header = NULL;
    asprintf(&header, "%-25s | %15s | %12s\n","Item", "Value (Gold)", "Weight (Stone)");
    printF(header);
    free(header);
    printF("-------------------------------------------------------\n");



    for (i = 0; i < total_products; i++) {
        char *output;
        int len = asprintf(&output, "%-25s | %15d | %12.1f\n", products[i].name, products[i].amount, products[i].weight);
        if(len == -1) {
            printF("Error creating output string\n");
            return;
        }
        printF(output);
        free(output);
    }
    printF("\n-------------------------------------------------------\n");  
    int len = asprintf(&output, "Total Entries: %d\n", total_products);
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

