#include "trade.h"


int valid_product(char *product) {
    char *product_list[] = {
        "MYRISH LACE",
        "SWEETWINE",
        "ARBOR GOLD",
        "DRAGON GLASS",
        "VALYRIAN STEEL"
    };

    for (int i = 0; i < 5; i++) {
        if (strcmp(product, product_list[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

int find_product(TradeItem *items, int count, char *product) {
    for (int i = 0; i < count; i++) {
        if (strcmp(items[i].name, product) == 0) {
            return i;
        }
    }
    return -1;
}

int is_number(char *str) {
    int i = 0;

    if (str == NULL || str[0] == '\0') {
        return 0;
    }

    while (str[i] != '\0') {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
        i++;
    }

    return 1;
}

void free_trade_items(TradeItem *items, int count) {
    for (int i = 0; i < count; i++) {
        free(items[i].name);
    }
    free(items);
}

void start_trade(char *realm) {
    char *command = NULL;
    char *output = NULL;
    char *filename = "trade_list.txt";

    TradeItem *selected_products = NULL;
    int selected_count = 0;

    int len = asprintf(&output, "Starting trade with %s...\n", realm);
    if (len != -1) {
        printF(output);
        free(output);
    }

    printF("Available products: Myrish Lace, Sweetwine, Arbor Gold, Dragon Glass, Valyrian Steel\n");

    while (1) {
        printF("(trade)> ");
        command = read_screen();
        if (command == NULL) {
            free_trade_items(selected_products, selected_count);
            return;
        }

        to_upper(command);

        int cmd_len = strlen(command);
        while (cmd_len > 0 && command[cmd_len - 1] == '\n') {
            command[cmd_len - 1] = '\0';
            cmd_len--;
        }

        if (strcmp(command, "CANCEL") == 0) {
            printF("Trade cancelled.\n");
            free(command);
            free_trade_items(selected_products, selected_count);
            return;
        }

        if (strcmp(command, "SEND") == 0) {
            int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                printF("Error: Could not open trade list file.\n");
                free(command);
                free_trade_items(selected_products, selected_count);
                return;
            }

            for (int i = 0; i < selected_count; i++) {
                char *line = NULL;
                int line_len = asprintf(&line, "%s %d\n", selected_products[i].name, selected_products[i].amount);
                if (line_len == -1) {
                    printF("Error: Could not write item.\n");
                    close(fd);
                    free(command);
                    free_trade_items(selected_products, selected_count);
                    return;
                }

                if (write(fd, line, line_len) == -1) {
                    printF("Error: Could not write to trade list file.\n");
                    free(line);
                    close(fd);
                    free(command);
                    free_trade_items(selected_products, selected_count);
                    return;
                }

                free(line);
            }

            close(fd);

            len = asprintf(&output, "Trade list sent to %s.\n", realm);
            if (len != -1) {
                printF(output);
                free(output);
            }

            free(command);
            free_trade_items(selected_products, selected_count);
            return;
        }

        char *w1, *w2, *w3, *w4;
        char *item = NULL;
        int amount = 0;

        w1 = strtok(command, " ");
        w2 = strtok(NULL, " ");
        w3 = strtok(NULL, " ");
        w4 = strtok(NULL, " ");

        if (w1 == NULL) {
            printF("Unknown command. Available commands: ADD <PRODUCT> <AMOUNT>, REMOVE <PRODUCT> <AMOUNT>, SEND, CANCEL\n");
            free(command);
            continue;
        }

        if (strcmp(w1, "ADD") == 0 || strcmp(w1, "REMOVE") == 0) {
            if (w2 == NULL || w3 == NULL) {
                printF("Invalid command. Usage: ADD/REMOVE <PRODUCT> <AMOUNT>\n");
                free(command);
                continue;
            }

            if (is_number(w3)) {
                if (w4 != NULL) {
                    printF("Invalid command. Usage: ADD/REMOVE <PRODUCT> <AMOUNT>\n");
                    free(command);
                    continue;
                }

                item = strdup(w2);
                amount = atoi(w3);
            } else {
                if (w4 == NULL || !is_number(w4)) {
                    printF("Invalid command. Usage: ADD/REMOVE <PRODUCT> <AMOUNT>\n");
                    free(command);
                    continue;
                }

                item = malloc(strlen(w2) + strlen(w3) + 2);
                if (item == NULL) {
                    printF("Error: Could not allocate memory for item.\n");
                    free(command);
                    free_trade_items(selected_products, selected_count);
                    return;
                }

                strcpy(item, w2);
                strcat(item, " ");
                strcat(item, w3);
                amount = atoi(w4);
            }

            if (amount <= 0) {
                printF("Invalid amount. Must be a positive integer.\n");
                free(item);
                free(command);
                continue;
            }

            if (!valid_product(item)) {
                printF("Invalid product. Available products: Myrish Lace, Sweetwine, Arbor Gold, Dragon Glass, Valyrian Steel\n");
                free(item);
                free(command);
                continue;
            }

            if (strcmp(w1, "ADD") == 0) {
                int index = find_product(selected_products, selected_count, item);

                if (index != -1) {
                    selected_products[index].amount += amount;
                } else {
                    TradeItem *new_items = realloc(selected_products, (selected_count + 1) * sizeof(TradeItem));
                    if (new_items == NULL) {
                        printF("Error: Could not allocate memory for trade items.\n");
                        free(item);
                        free(command);
                        free_trade_items(selected_products, selected_count);
                        return;
                    }

                    selected_products = new_items;
                    selected_products[selected_count].name = strdup(item);
                    if (selected_products[selected_count].name == NULL) {
                        printF("Error: Could not allocate memory for product name.\n");
                        free(item);
                        free(command);
                        free_trade_items(selected_products, selected_count);
                        return;
                    }

                    selected_products[selected_count].amount = amount;
                    selected_count++;
                }

                printF("Product added to trade list.\n");
            } else if (strcmp(w1, "REMOVE") == 0) {
                int index = find_product(selected_products, selected_count, item);

                if (index == -1) {
                    printF("Product not in trade list.\n");
                } else if (selected_products[index].amount < amount) {
                    printF("Cannot remove more than the current amount in trade list.\n");
                } else {
                    selected_products[index].amount -= amount;

                    if (selected_products[index].amount == 0) {
                        free(selected_products[index].name);

                        for (int i = index; i < selected_count - 1; i++) {
                            selected_products[i] = selected_products[i + 1];
                        }

                        selected_count--;

                        if (selected_count == 0) {
                            free(selected_products);
                            selected_products = NULL;
                        } else {
                            TradeItem *new_items = realloc(selected_products, selected_count * sizeof(TradeItem));
                            if (new_items != NULL) {
                                selected_products = new_items;
                            }
                        }
                    }

                    printF("Product removed from trade list.\n");
                }
            }

            free(item);
            free(command);
            continue;
        }

        printF("Unknown command. Available commands: ADD <PRODUCT> <AMOUNT>, REMOVE <PRODUCT> <AMOUNT>, SEND, CANCEL\n");
        free(command);
    }
}
