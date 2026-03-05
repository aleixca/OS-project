#include "inventory.h"



void load_inventory(char *argv[]) {

    int fd = open(argv[2], O_RDONLY);
    int i = 0;
    int n;
    int total_products = 0;
    Product *products;

    if (fd == -1) {
        printF("Error: Could not open inventory file.\n");
        return;
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
            return;
        }
        products = new_products;
        products[i] = temp;
        i++;
    }
    total_products = i;
    close(fd);
}

void list_products(int total_products, Product *products) {
    int i = 0;
    printF("--- Trade Ledger --- \n");
    printF("Item                    | Value (Gold) | Weight (Stone)\n");
    PrintF("-------------------------------------------------------\n");

    for (i = 0; i < total_products; i++) {
        char *output;
        int len = asprintf(&output, "\n %s      %d      %.1f ", products[i].name, products[i].amount, products[i].weight);
        write(1, output, len);
        free(output);
    }
    printF("\n-------------------------------------------------------\n");
    printF("Total Entries: %d\n", total_products);

}

void free_inventory(Product *products) {
    free(products);
    products = NULL;
}
