#ifndef COMMANDS_H
#define COMMANDS_H

#define CMD_UNKNOWN            0
#define CMD_LIST_REALMS        1
#define CMD_LIST_PRODUCTS      2
#define CMD_START_TRADE        3
#define CMD_EXIT               4
#define CMD_OK_OTHER           5
#define CMD_INCOMPLETE         6


int parse_command(char **argv, int argc);

#endif
