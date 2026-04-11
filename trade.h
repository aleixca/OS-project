#ifndef TRADE_H
#define TRADE_H
#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "io.h"
#include "types.h"
#include "network.h"

typedef struct {
    char *name;
    int amount;
} TradeItem;

void start_trade(char *realm, Maester maester);

#endif
