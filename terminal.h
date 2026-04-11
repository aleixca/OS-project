#ifndef TERMINAL_H
#define TERMINAL_H

#include "commands.h"
#include "io.h"
#include "signal.h"
#include "types.h"
#include "inventory.h"
#include "trade.h"
#include "pledge_network.h"


#include <string.h>
#include <unistd.h>

void terminal(int total_products, Product *products, Maester maester);
void list_realms(Maester maester);
void exit_maester(Maester maester);

#endif
