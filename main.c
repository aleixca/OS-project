#define _GNU_SOURCE
#include "terminal.h"
#include "signal.h"
#include "io.h"
#include "maester.h"
#include "inventory.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




int main(int argc, char *argv[]){
    int total_products = 0;
    Product *products = NULL;
    Maester maester;


    if (argc != 3) {
        printF("Usage: ./main <maester_config.txt> <inventory.bin>\n");
        return 1;
    }

    
        setup_signal();
        maester = read_Maester(argv[1]);
        products = load_inventory(argv[2], &total_products);
        terminal(total_products, products, maester);
    
    free_inventory(products);
    free_Maester(maester);
    

    return 0;
}
