#include "terminal.h"


/********************
 *
 * @Name: terminal
 * @Def: Main terminal interface for user interaction.
 * @Arg: total_products = total number of products
 *       products = pointer to the array of products
 *       maester = the Maester instance
 * @Ret: None
 *
 ********************/
void terminal(int total_products, Product *products, Maester maester) {
    int exit = 0;
    char *realm = NULL;

    while (!stop_requested() && !exit) {

    switch (parse_command(&realm)) {
        case CMD_LIST_REALMS:
            list_realms(maester);
            break;
        case CMD_LIST_PRODUCTS_OWN:
            list_products(total_products, products);
            break;
        case CMD_LIST_PRODUCTS_REALM:
            printF("Command ok\n");
            break;
        case CMD_START_TRADE:
            start_trade(realm, maester);
            free(realm);
            break;
        case CMD_PLEDGE_STATUS:
            show_pledge_status();
            break;
        case CMD_ENVOY_STATUS:
            printF("Command ok.\n");
            break;
        case CMD_PLEDGE_RESPOND_ACCEPT:
            printF("Command ok.\n");
            break;
        case CMD_PLEDGE_RESPOND_REJECT:
            printF("Command ok.\n");
            break;
        case CMD_PLEDGE:
            handle_pledge(realm, maester);
            free(realm);
            break;
        case CMD_UNKNOWN:
            printF("Unknown command.\n");
            break;
        case CMD_INCOMPLETE_LIST:
            printF("Incomplete command: LIST requires additional arguments. (LIST REALMS or LIST PRODUCTS)\n");
            break;
        case CMD_INCOMPLETE_PLEDGE:
            printF("Incomplete command: PLEDGE requires additional arguments.(PLEDGE STATUS or PLEDGE RESPOND <REALM> ACCEPT/REJECT)\n");
            break;
        case CMD_INCOMPLETE_PLEDGE_RESPOND:
            printF("Incomplete command: PLEDGE RESPOND requires additional arguments.(PLEDGE RESPOND <REALM> ACCEPT/REJECT)\n");
            break;
        case CMD_INCOMPLETE_ALLIANCE:
            printF("Incomplete command: ALLIANCE requires additional arguments. (PLEDGE <REALM> <sigil.jpg>)\n");
            break;
        case CMD_INCOMPLETE_TRADE:
            printF("Incomplete command: TRADE requires additional arguments. (START TRADE <REALM>)\n");
            break;
        case CMD_INCOMPLETE_ENVOY:
            printF("Incomplete command: ENVOY requires additional arguments. (ENVOY STATUS)\n");
            break;
        case CMD_EXIT:
            free(realm);
            exit_maester(maester);
            exit = 1;
            break;
    }
} 

}
