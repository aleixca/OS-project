#include "commands.h"


int parse_command(char **realm) {
    char *command = NULL;
    char *w1, *w2, *w3, *w4, *w5;
    int result = CMD_UNKNOWN;
    
    
    if (realm != NULL) {
        *realm = NULL;
    }

    printF("$ ");
    command = read_screen();

    if (command == NULL) {
        return CMD_EXIT;
    }

    if (strlen(command) == 0) {
        free(command);
        return CMD_UNKNOWN;
    }

    to_upper(command);

    w1 = strtok(command, " ");
    w2 = strtok(NULL, " ");
    w3 = strtok(NULL, " ");
    w4 = strtok(NULL, " ");
    w5 = strtok(NULL, " ");

    if (w1 == NULL) {
        result = CMD_UNKNOWN;
    }

    else if (strcmp(w1, "EXIT") == 0) {
        result =  CMD_EXIT;
    }

    else if (strcmp(w1, "LIST") == 0) {
        if (w2 == NULL) {
            result = CMD_INCOMPLETE_LIST;
        } else if (strcmp(w2, "REALMS") == 0) {
            if (w3 != NULL) {
                result = CMD_UNKNOWN;
            } else {
                result = CMD_LIST_REALMS;
            }
        } else if (strcmp(w2, "PRODUCTS") == 0) {
            if (w3 == NULL) {
                result = CMD_LIST_PRODUCTS_OWN;
            } else if (w4 == NULL) {
                result = CMD_LIST_PRODUCTS_REALM;
            } else {
                result = CMD_UNKNOWN;
            }
        } else {
            result = CMD_UNKNOWN;
        }
    }

    else if (strcmp(w1, "PLEDGE") == 0) {
        if (w2 == NULL) {
            result = CMD_INCOMPLETE_PLEDGE;
        } else if (strcmp(w2, "STATUS") == 0) {
            if (w3 != NULL) {
                result = CMD_UNKNOWN;
            } else {
                result = CMD_PLEDGE_STATUS;
            }
        } else if (strcmp(w2, "RESPOND") == 0) {
            if (w3 == NULL || w4 == NULL) {
                result = CMD_INCOMPLETE_PLEDGE_RESPOND;
            } else if (w5 != NULL) {
                result = CMD_UNKNOWN;
            } else if (strcmp(w4, "ACCEPT") == 0) {
                result = CMD_PLEDGE_RESPOND_ACCEPT;
            } else if (strcmp(w4, "REJECT") == 0) {
                result = CMD_PLEDGE_RESPOND_REJECT;
            } else {
                result = CMD_UNKNOWN;
            }
        } else {
           
            if (w3 == NULL) {
                result = CMD_INCOMPLETE_ALLIANCE;
            } else if (w4 != NULL) {
                result = CMD_UNKNOWN;
            } else {
                result = CMD_PLEDGE;
            }
        }
    }

    else if (strcmp(w1, "START") == 0) {
        if (w2 == NULL) {
            result = CMD_INCOMPLETE_TRADE;
        } else if (strcmp(w2, "TRADE") != 0) {
            result = CMD_UNKNOWN;
        } else if (w3 == NULL) {
            result = CMD_INCOMPLETE_TRADE;
        } else if (w4 != NULL) {
            result = CMD_UNKNOWN;
        } else {
            
            result = CMD_START_TRADE;
            *realm = strdup(w3);
        }
    }

    else if (strcmp(w1, "ENVOY") == 0) {
        if (w2 == NULL) {
            result = CMD_INCOMPLETE_ENVOY;
        } else if (strcmp(w2, "STATUS") == 0 && w3 == NULL) {
            result = CMD_ENVOY_STATUS;
        } else {
            result = CMD_UNKNOWN;
        }
    }

    else {
        result = CMD_UNKNOWN;
    }

    free(command);
    return result;
}
