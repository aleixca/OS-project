#include "signal.h"


int stop_program = 0;

/********************
 *
 * @Name: signal_handler
 * @Def: Handles incoming signals.
 * @Arg: sig = signal number
 * @Ret: None
 *
 ********************/
void signal_handler(int sig) {
    (void)sig;
    stop_program = 1;
    close(STDIN_FILENO);
}

/********************
 *
 * @Name: setup_signal
 * @Def: Sets up the signal handler for the program.
 * @Arg: None
 * @Ret: None
 *
 ********************/
void setup_signal(void) {
    signal(SIGINT, signal_handler);
}

/********************
 *
 * @Name: stop_requested
 * @Def: Checks if a stop has been requested.
 * @Arg: None
 * @Ret: 1 if stop requested, 0 otherwise
 *
 ********************/
int stop_requested(void) {
    return stop_program != 0;
}


