#ifndef TERMINAL_H
#define TERMINAL_H

#include "commands.h"
#include "io.h"
#include "signal.h"
#include "maester.h"
#include "inventory.h"
#include "pledge.h"


#include <string.h>
#include <unistd.h>

void terminal(int total_products, Product *products, Maester maester);

#endif
