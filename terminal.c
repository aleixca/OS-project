#include "terminal.h"

#include <stdlib.h>

void terminal(int total_products, Product *products, Maester maester) {
    char *command;
    int running = 1;

    setup_signal();

    while (running && !stop_requested()) {
        printF("$ ");
        command = read_screen();

        switch (parse_command(command)) {
            case CMD_LIST_REALMS:
                list_realms(maester);
                printF("\n");
                break;
            case CMD_LIST_PRODUCTS:
                list_products(total_products, products);
                break;
            case CMD_LIST_PRODUCTS_REALM:
                printF("ERROR: You must have an alliance to trade.\n");
                break;
            case CMD_PLEDGE:
                printF("Pledge sent.\n");
                break;
            case CMD_PLEDGE_STATUS:
                printF("$ PLEDGE STATUS\n");
                break;
            case CMD_START_TRADE_REALM:
                printF("Entering trade mode.\n");
                break;
            case CMD_START_TRADE:
                printF("Did you mean to start a trade? Please review syntax: START TRADE <REALM>\n");
                break;
            case CMD_INCOMPLETE:
                printF("Did you mean to send a pledge? Please review syntax.\n");
                break;
            case CMD_EXIT:
                printF("Exiting...\n");
                running = 0;
                break;
            default:
                printF("Unknown command.\n");
                break;
        }

        free(command);
    }
}