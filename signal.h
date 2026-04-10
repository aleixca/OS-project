#ifndef SIGNAL_H
#define SIGNAL_H

#include "io.h"

#include <string.h>
#include <unistd.h>


#include <signal.h>


void setup_signal(void);
int  stop_requested(void);

#endif
