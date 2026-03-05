#include "commands.h"


int parse_command(char *command){
    
    if (strlen(command) == 0) {
        return CMD_UNKNOWN;
    }

    to_upper(command);

    if (strcmp(command, "LIST REALMS") == 0) {
        return CMD_LIST_REALMS;
    } else if (strcmp(command, "LIST PRODUCTS") == 0) {
        return CMD_LIST_PRODUCTS;
    } else if (strcmp(command, "START TRADE") == 0) {
        return CMD_START_TRADE;
    } else if (strcmp(command, "EXIT") == 0) {
        return CMD_EXIT;
    } else { // AFEGIR ELS COMMANDS QUE HAN DE FER RETURN DE CMD_OK_OTHER I CMD_INCOMPLETE
        return CMD_UNKNOWN;
    }
}
