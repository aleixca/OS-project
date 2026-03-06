#include "commands.h"

int parse_command(char *command){
    char *upper_cmd;
    
    if (strlen(command) == 0) {
        return CMD_UNKNOWN;
    }

    upper_cmd = strdup(command);
    to_upper(upper_cmd);

    if (strcmp(upper_cmd, "LIST REALMS") == 0) {
        free(upper_cmd);
        return CMD_LIST_REALMS;
    } else if (strcmp(upper_cmd, "PLEDGE STATUS") == 0) {
        free(upper_cmd);
        return CMD_PLEDGE_STATUS;
    } else if (strncmp(upper_cmd, "PLEDGE ", 7) == 0) {
        free(upper_cmd);
        return CMD_PLEDGE;
    } else if (strcmp(upper_cmd, "PLEDGE") == 0) {
        free(upper_cmd);
        return CMD_INCOMPLETE;
    } else if (strncmp(upper_cmd, "LIST PRODUCTS ", 14) == 0) {
        free(upper_cmd);
        return CMD_LIST_PRODUCTS_REALM;
    } else if (strcmp(upper_cmd, "LIST PRODUCTS") == 0) {
        free(upper_cmd);
        return CMD_LIST_PRODUCTS;
    } else if (strncmp(upper_cmd, "START TRADE ", 12) == 0) {
        free(upper_cmd);
        return CMD_START_TRADE_REALM;
    } else if (strcmp(upper_cmd, "START TRADE") == 0) {
        free(upper_cmd);
        return CMD_START_TRADE;
    } else if (strcmp(upper_cmd, "EXIT") == 0) {
        free(upper_cmd);
        return CMD_EXIT;
    } else {
        free(upper_cmd);
        return CMD_UNKNOWN;
    }
}