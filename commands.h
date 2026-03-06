#ifndef COMMANDS_H
#define COMMANDS_H

#include <string.h>
#include <ctype.h>

#define CMD_UNKNOWN            0
#define CMD_LIST_REALMS        1
#define CMD_LIST_PRODUCTS      2
#define CMD_START_TRADE        3
#define CMD_EXIT               4
#define CMD_OK                 5
#define CMD_INCOMPLETE         6
#define CMD_PLEDGE             7
#define CMD_PLEDGE_STATUS      8
#define CMD_LIST_PRODUCTS_REALM 9
#define CMD_START_TRADE_REALM  10

int parse_command(char *command);

#endif