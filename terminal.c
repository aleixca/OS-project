#include "terminal.h"




void terminal() {


    while (!stop_program) {

    switch (command){
        case CMD_LIST_REALMS:
            list_realms(realm_count, routes);
            break;
        case CMD_LIST_PRODUCTS:
            list_products(total_products, products);
            break;
        case CMD_START_TRADE:
            start_trade();
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

}
