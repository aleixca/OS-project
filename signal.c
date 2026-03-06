#include "signal.h"


int stop_program = 0;

void signal_handler(int sig) {
    (void)sig;
    stop_program = 1;

    printF("\nExiting program gracefully...\n");
}

void setup_signal(void) {
    signal(SIGINT, signal_handler);
}

int stop_requested(void) {
    return stop_program != 0;
}


