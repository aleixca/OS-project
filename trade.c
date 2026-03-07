#include "trade.h"


void start_trade(char *realm) {
    char *command = NULL;
   char *product_list[] = {
    "MYRISH LACE\n",
    "SWEETWINE\n",
    "ARBOR GOLD\n",
    "DRAGON GLASS\n",
    "VALYRIAN STEEL\n"
};

    
    int fd = open("trade_list.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printF("Error: Could not open trade list file.\n");
        return;
     }

     char *output;

    int len = asprintf(&output, "Starting trade with %s...\n", realm);
    if (len != -1) {
        printF(output);
        free(output);
        
    }

    printF("Available products: Myrish Lace, Sweetwine, Arbor Gold, Dragon Glass, Valyrian Steel\n");


        printF("(trade) > ");
        command = read_screen();
        if (command == NULL) {
            close(fd);
            return;
        }
        to_upper(command);
        command = add_newline(command);
    while (strcmp(command, "SEND\n") != 0){
        
        for (int i = 0; i < 5; i++) {
            if (strcmp(command, product_list[i]) == 0 || (strcmp(command, "SEND\n") == 0)) {
                break;
            }
            if (i == 4) {
                i = 0;
                printF("Invalid product. Please enter a valid product name.\n");
                printF("(trade) > ");
                free(command);
                command = read_screen();
                if (command == NULL) {
                    close(fd);
                    return;
                }
                to_upper(command);
                command = add_newline(command);
            }
        }
        if (strcmp(command, "SEND\n") == 0) {
            break;
        }
        int n = write(fd, command, strlen(command));
        
        if (n == -1) {
            printF("Error: Could not write to trade list file.\n");
            free(command);
            close(fd);
            return;
        }
        free(command);
        printF("(trade) > ");
        command = read_screen();
        if (command == NULL) {
            close(fd);
            return;
        }
        to_upper(command);
        command = add_newline(command);
    }

    free(command);
    len = asprintf(&output, "Trade list sent to %s.\n", realm);
    if (len != -1) {
        printF(output);
        free(output);
    }
    close (fd);
}
