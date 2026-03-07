#ifndef TRADE_H
#define TRADE_H
#define _GNU_SOURCE

#include "io.h"
#include "maester.h"
#include "inventory.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


void start_trade(char *realm);

#endif
