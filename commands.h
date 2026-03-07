#ifndef COMMANDS_H
#define COMMANDS_H

#include "io.h"
#include "signal.h"

#include <string.h>
#include <ctype.h>

#define CMD_UNKNOWN 0
#define CMD_LIST_REALMS 1
#define CMD_PLEDGE 2
#define CMD_PLEDGE_RESPOND_ACCEPT 3
#define CMD_PLEDGE_RESPOND_REJECT 4
#define CMD_LIST_PRODUCTS_OWN 5
#define CMD_LIST_PRODUCTS_REALM 6
#define CMD_START_TRADE 7
#define CMD_PLEDGE_STATUS 8
#define CMD_ENVOY_STATUS 9
#define CMD_EXIT 10
#define CMD_INCOMPLETE_LIST 11
#define CMD_INCOMPLETE_PLEDGE 12
#define CMD_INCOMPLETE_PLEDGE_RESPOND 13
#define CMD_INCOMPLETE_ALLIANCE 14
#define CMD_INCOMPLETE_TRADE 15
#define CMD_INCOMPLETE_ENVOY 16



int parse_command(char **realm);

#endif
