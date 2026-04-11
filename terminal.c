#include "terminal.h"

/********************
 *
 * @Name: check_incoming
 * @Def: Checks for an incoming connection on the listening socket
 *       using non-blocking accept. If a frame arrives, dispatches it.
 * @Arg: maester = the Maester instance
 * @Ret: None
 *
 ********************/
static void check_incoming(Maester maester) {
    Frame frame;
    int client_fd;
 
    client_fd = accept_connection(maester.listen_fd);
    if (client_fd < 0) {
        return;
    }
 
    if (!receive_frame(client_fd, &frame)) {
        close_connection(client_fd);
        return;
    }
 
    if (frame.type == TYPE_ALLIANCE_REQUEST) {
        handle_incoming_pledge(client_fd, &frame);
    } else {
        close_connection(client_fd);
    }
}

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
        check_incoming(maester);

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
            handle_pledge_respond(realm, maester, 1);
            free(realm);
            break;
        case CMD_PLEDGE_RESPOND_REJECT:
            handle_pledge_respond(realm, maester, 0);
            free(realm);
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
