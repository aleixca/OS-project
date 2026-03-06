#include "terminal.h"




void terminal(int total_products, Product *products, Maester maester) {

    
    setup_signal();

    while (!stop_requested()) {

    switch (parse_command()) {
        case CMD_LIST_REALMS:
            list_realms(maester);
            break;
        case CMD_LIST_PRODUCTS:
            list_products(total_products, products);
            break;
        case CMD_START_TRADE:
            printF("Starting trade...\n");
            break;
        case CMD_OK:
            printF("Command executed successfully.\n");
            break;
        case CMD_INCOMPLETE:
            printF("Command is incomplete. Please provide more information.\n");
            break;
        case CMD_EXIT:
            printF("Exiting...\n");
            break;
        default:
            printF("Unknown command.\n");
            break;
    }
} 

    void free_inventory(Product *products);


}
